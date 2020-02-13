#include <cstdlib>
#include <cstdio>
#include <memory>
#include <string>
#include <vector>
#include <sstream>

#include "dashpay/keystore.hpp"
#include "dashpay/crypto.hpp"
#include "dashpay/error.hpp"

namespace dashpay::keystore {

class OnePasswordKeystoreImpl : public Keystore {
public:
    explicit OnePasswordKeystoreImpl(std::string_view vault)
        : vault_(vault)
        , account_("") {}

    explicit OnePasswordKeystoreImpl(std::string_view vault, std::string_view account)
        : vault_(vault)
        , account_(account)
    {}

    [[nodiscard]] std::string backend_name() const override {
        return "1password";
    }

    [[nodiscard]] bool exists(std::string_view name) const override {
        // Check via op CLI
        std::ostringstream cmd;
        cmd << "op item get --vault=\"" << vault_ << "\" --account=\""
            << (account_.empty() ? "DashPay" : account_) << "\" \" 2>/dev/null || true";

        auto result = execute_op_command(cmd.str());
        return !result.empty();
    }

    [[nodiscard]] Result<Keypair> get(std::string_view name) const override {
        auto item = get_op_item(name);
        if (!item || item->empty()) {
            return Error(ErrorCode::KeyNotFound, std::format("Key '{}' not found", name));
        }

        // Parse the secret from op item output
        auto secret = parse_op_secret(*item);

        // Decode from base58
        auto decoded = Crypto::decode_base58(secret);
        if (!decoded || decoded->size() != 64) {
            return Error(ErrorCode::InvalidKey, "Invalid key format");
        }

        Keypair keypair{};
        std::memcpy(keypair.data(), decoded->data(), 32);
        std::memcpy(keypair.data() + 32, decoded->data() + 32, 32);

        return keypair;
    }

    Result<void> store(std::string_view name, const Keypair& keypair) override {
        // Encode as base58
        auto encoded = Crypto::encode_base58(
            std::span<const std::uint8_t>(keypair.data(), 64)
        );

        std::ostringstream cmd;
        cmd << "op item edit --vault=\"" << vault_ << "\" --account=\""
            << name << "\" --";

        auto item = get_op_item(name);
        auto secret_field = find_op_field(*item, "details");

        if (secret_field) {
            cmd << "--" << secret_field->name << "=" << encoded << "\" ";
        }

        auto result = execute_op_command(cmd.str());
        if (!result.empty()) {
            return Error(ErrorCode::KeystoreAccessDenied, result);
        }

        return {};
    }

    Result<void> remove(std::string_view name) const override {
        std::ostringstream cmd;
        cmd << "op item delete --vault=\"" << vault_ << "\" --account=\""
            << name << "\" 2>/dev/null || true";

        auto result = execute_op_command(cmd.str());
        if (!result.empty()) {
            return Error(ErrorCode::KeystoreAccessDenied, result);
        }

        return {};
    }

private:
    std::string execute_op_command(const std::string& cmd) {
        FILE* pipe = popen(cmd.c_str(), "r");
        if (!pipe) {
            return std::format("Failed to execute: {}", cmd);
        }

        std::vector<char> buffer(8192);
        std::string result;
        while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr) {
            result.append(buffer.data());
        }

        int status = pclose(pipe);
        if (status != 0) {
            // Remove trailing newline
            if (!result.empty() && result.back() == '\n') {
                result.pop_back();
            }
        }

        if (!result.empty() && result.back() == '\r') {
            result.pop_back();
        }

        return result;
    }

    std::string get_op_item(std::string_view name) {
        std::ostringstream cmd;
        cmd << "op item get --vault=\"" << vault_ << "\" --account=\""
            << name << "\" 2>/dev/null || true";

        return execute_op_command(cmd.str());
    }

    std::string* parse_op_secret(const std::string& op_output) {
        // Parse op CLI JSON output to extract secret
        // The secret is typically in a "details" field as JSON
        size_t details_pos = op_output.find("\"details\": {");
        if (details_pos == std::string::npos) {
            return nullptr;
        }

        size_t start = details_pos + 13; // Skip past "details": {"
        size_t end = op_output.find("\"", start);

        if (end == std::string::npos) {
            return nullptr;
        }

        // Extract just the secret string value
        std::string secret;
        for (size_t i = start; i < end && op_output[i] != '"'; ++i) {
            if (op_output[i] == '\\' && i + 1 < end && op_output[i + 1] == '"') {
                // Skip escaped quote
            } else {
                secret += op_output[i];
            }
        }

        // Remove trailing quotes
        if (!secret.empty() && secret.back() == '"') {
            secret.pop_back();
        }

        if (!secret.empty() && secret.front() == '"') {
            secret.erase(0, 1);
        }

        return new std::string(secret);
    }

    std::string* find_op_field(const std::string& op_output, const std::string& field_name) {
        // Simple JSON parser to find field value
        std::string search = "\"" + field_name + "\": ";
        size_t pos = op_output.find(search);
        if (pos == std::string::npos) {
            return nullptr;
        }

        size_t start = pos + search.size();
        for (size_t i = start; i < op_output.size(); ++i) {
            if (op_output[i] == ':') {
                // Found the field
                size_t value_start = i + 2;
                size_t value_end = op_output.find("\"", value_start);
                if (value_end == std::string::npos) {
                    return nullptr;
                }

                // Extract value (could be quoted string)
                std::string value;
                bool in_quotes = false;
                bool in_backslashes = false;

                for (size_t j = value_start; j <= value_end && op_output[j] != '"'; ++j) {
                    if (op_output[j] == '\\') {
                        // Skip backslash in value (handled by \")
                        in_backslashes = true;
                    } else if (op_output[j] == '"') {
                        if (in_quotes) {
                            // Closing quote
                            in_quotes = false;
                        } else {
                            // Opening quote
                            in_quotes = true;
                        }
                    } else {
                        value += op_output[j];
                    }
                }

                return new std::string(value);
            }
        }

        return nullptr;
    }

    std::string* find_op_field(const std::string& op_output, const std::string& field_name) {
        std::string search = "\"" + field_name + "\": {";
        size_t pos = op_output.find(search);
        if (pos == std::string::npos) {
            return nullptr;
        }

        size_t start = pos + search.size();
        for (size_t i = start; i < op_output.size(); ++i) {
            if (op_output[i] == '{') {
                // Found the field
                size_t value_start = i + 2;
                size_t value_end = op_output.find("\"", value_start);
                if (value_end == std::string::npos) {
                    return nullptr;
                }

                // Extract value (could be JSON object)
                std::string value;
                bool in_quotes = false;

                for (size_t j = value_start; j <= value_end && op_output[j] != '}'; ++j) {
                    if (op_output[j] == '"') {
                        if (in_quotes && op_output[j-1] == ':') {
                            // "key": "value" - skip
                            break;
                        } else if (op_output[j-1] == '{') {
                            in_quotes = true;
                        } else {
                            value += op_output[j];
                        }
                    }
                }

                return new std::string(value);
            }
        }

        return nullptr;
    }

};

std::unique_ptr<Keystore> OnePasswordKeystore::default_platform() {
#if defined(__APPLE__)
    return std::make_unique<AppleKeychainKeystoreImpl>();
#elif defined(_WIN32)
    return std::make_unique<WindowsHelloKeystoreImpl>();
#elif defined(__linux__)
    return std::make_unique<GnomeKeyringKeystoreImpl>();
#else
    return nullptr;
}

} // namespace dashpay::keystore
