#pragma once

#include <cstdint>
#include <expected>
#include <string>
#include <vector>

#include "dashpay/error.hpp"
#include "dashpay/types.hpp"

namespace dashpay {

struct RpcRequest {
    std::string jsonrpc = "2.0";
    std::string method;
    std::string id;
    std::string params;
};

struct RpcResponse {
    int code;
    std::string message;
    std::string data;
};

class RpcClient {
public:
    explicit RpcClient(std::string_view url);

    [[nodiscard]] Result<uint64_t> get_balance(std::string_view pubkey);
    [[nodiscard]] Result<std::string> get_signature_status(std::string_view signature);
    [[nodiscard]] Result<uint64_t> get_slot() const;

    [[nodiscard]] Result<std::string> send_transaction(
        const std::string& transaction,
        std::string_view fee_payer
    );

    [[nodiscard]] Result<std::vector<uint8_t>> get_account_info(
        std::string_view pubkey
    );

private:
    std::string url_;
    uint64_t timeout_ms_ = 30000;
};

class RpcRequestBuilder {
public:
    [[nodiscard]] static std::string get_balance(std::string_view pubkey);
    [[nodiscard]] static std::string send_transaction(const std::string& encoded_tx);
    [[nodiscard]] static std::string get_signature_status(std::string_view signature);
    [[nodiscard]] static std::string get_slot();
};

} // namespace dashpay
