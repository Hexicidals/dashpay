#include <curl/curl.h>
#include <thread>
#include <mutex>
#include <memory>
#include <queue>
#include <sstream>
#include <string>

#include "dashpay/rpc.hpp"
#include "dashpay/error.hpp"
#include "dashpay/types.hpp"

namespace dashpay {

class RpcClientImpl : public RpcClient {
public:
    explicit RpcClientImpl(std::string_view url)
        : rpc_url_(url),
          timeout_ms_(30000),
          session_token_(std::nullopt)
    {
        curl_ = curl_easy_init();
        if (!curl_) {
            throw std::runtime_error("Failed to initialize curl");
        }

        curl_easy_setopt(curl_, CURLOPT_URL, url.data());

        // Set timeout
        curl_easy_setopt(curl_, CURLOPT_TIMEOUT, static_cast<long>(timeout_ms_ / 1000));

        // Follow redirects
        curl_easy_setopt(curl_, CURLOPT_FOLLOWLOCATION, 1L);

        // Enable TCP keepalive
        curl_easy_setopt(curl_, CURLOPT_TCP_KEEPALIVE, 1L);
        curl_easy_setopt(curl_, CURLOPT_TCP_KEEPIDLE, 60L);
        curl_easy_setopt(curl_, CURLOPT_TCP_KEEPINTVL, 30L);
    }

    ~RpcClientImpl() override {
        if (curl_) {
            curl_easy_cleanup(curl_);
        }
    }

    void set_session_token(std::string_view token) override {
        std::lock_guard<std::mutex> lock(mutex_);
        session_token_ = token;
        struct curl_slist* headers = nullptr;
        struct curl_slist* item = curl_slist_append(headers, "Authorization: Bearer ");
        item = curl_slist_append(item, token);
        curl_slist_free(item);
        curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, headers);
    }

    void clear_session_token() override {
        std::lock_guard<std::mutex> lock(mutex_);
        session_token_ = std::nullopt;
        curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, nullptr);
    }

    Result<uint64_t> get_slot() const override {
        auto result = send_rpc_request({
            "jsonrpc": "2.0",
            "id": "1",
            "method": "getSlot"
        });

        if (!result) {
            return std::unexpected(result.error());
        }

        // Parse response
        auto json_result = parse_json_response(result->data);
        if (!json_result || !json_result->contains("result")) {
            return Error(ErrorCode::SolanaRpcError, "Invalid getSlot response");
        }

        uint64_t slot = std::stoull(json_result->at("result"));
        return slot;
    }

    Result<uint64_t> get_balance(std::string_view pubkey) override {
        auto result = send_rpc_request({
            "jsonrpc": "2.0",
            "id": "2",
            "method": "getBalance",
            "params": [std::format(R"[{}]", pubkey)]
        });

        if (!result) {
            return std::unexpected(result.error());
        }

        auto json_result = parse_json_response(result->data);
        if (!json_result || !json_result->contains("result")) {
            return Error(ErrorCode::SolanaRpcError, "Invalid getBalance response");
        }

        const auto& value = json_result->at("result");
        // Balance in lamports
        uint64_t lamports = std::stoull(value);
        return lamports;
    }

    Result<std::string> get_signature_status(std::string_view signature) override {
        auto result = send_rpc_request({
            "jsonrpc": "2.0",
            "id": "3",
            "method": "getSignatureStatus",
            "params": [std::format(R"{}", signature)]
        });

        if (!result) {
            return std::unexpected(result.error());
        }

        auto json_result = parse_json_response(result->data);
        if (!json_result || !json_result->contains("result")) {
            return Error(ErrorCode::SolanaRpcError, "Invalid getSignatureStatus response");
        }

        return json_result->at("result");
    }

    Result<std::string> send_transaction(
        const std::string& encoded_tx,
        std::string_view fee_payer
    ) override {
        auto result = send_rpc_request({
            "jsonrpc": "2.0",
            "id": "4",
            "method": "sendTransaction",
            "params": [
                encoded_tx,
                fee_payer
            ]
        });

        if (!result) {
            return std::unexpected(result.error());
        }

        auto json_result = parse_json_response(result->data);
        if (!json_result || !json_result->contains("result")) {
            return Error(ErrorCode::SolanaRpcError, "Invalid sendTransaction response");
        }

        return json_result->at("result");
    }

    Result<std::vector<uint8_t>> get_account_info(std::string_view pubkey) override {
        auto result = send_rpc_request({
            "jsonrpc": "2.0",
            "id": "5",
            "method": "getAccountInfo",
            "params": [std::format(R"[{}]", pubkey)]
        });

        if (!result) {
            return std::unexpected(result.error());
        }

        auto json_result = parse_json_response(result->data);
        if (!json_result || !json_result->contains("result")) {
            return Error(ErrorCode::SolanaRpcError, "Invalid getAccountInfo response");
        }

        const auto& value = json_result->at("result");
        if (value.is_string()) {
            return Error(ErrorCode::SolanaRpcError, "Unexpected account info type");
        }

        // Parse account info JSON
        std::vector<uint8_t> account_data = parse_base58(value.get_string());

        return account_data;
    }

private:
    [[nodiscard]] Result<std::string> send_rpc_request(const std::string& json_body) {
        std::string response_body;
        curl_slist* headers = nullptr;

        if (session_token_) {
            struct curl_slist* item = curl_slist_append(headers, "Authorization: Bearer ");
            item = curl_slist_append(item, *session_token_);
            curl_slist_free(item);
            curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, headers);
        }

        curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, json_body.data());

        // Set up response buffer
        std::string response;
        curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &response);

        // Perform request
        CURLcode res = curl_easy_perform(curl_);

        if (res != CURLE_OK) {
            return Error(ErrorCode::NetworkError,
                        std::format("Curl request failed: {}", curl_easy_strerror(res)));
        }

        return response;
    }

    [[nodiscard]] static Result<nlohmann::json> parse_json_response(const std::string& body) {
        try {
            return nlohmann::json::parse(body);
        } catch (const nlohmann::json::exception& e) {
            return Error(ErrorCode::SolanaRpcError,
                        std::format("JSON parse error: {}", e.what()));
        }
    }

    static size_t write_callback(
        void* contents,
        size_t size,
        size_t nmemb,
        void* userp
    ) {
        auto* str = static_cast<std::string*>(userp);
        str->append(static_cast<const char*>(contents), size);
        return size;
    }

    CURL* curl_;
    std::mutex mutex_;
};

std::unique_ptr<RpcClient> create_rpc_client(std::string_view url) {
    return std::make_unique<RpcClientImpl>(url);
}

} // namespace dashpay
