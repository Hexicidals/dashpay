#pragma once

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>

#include "dashpay/error.hpp"

namespace dashpay {

inline constexpr std::string_view LOCAL_RPC_URL = "http://localhost:8899";
inline constexpr std::string_view SANDBOX_RPC_URL = "https://402.surfnet.dev:8899";

enum class LogFormat : uint8_t {
    Text,
    Json,
};

struct Config {
    bool auto_pay = false;
    std::optional<std::string> keypair;
    std::optional<std::string> rpc_url;
    LogFormat log_format = LogFormat::Text;

    Config() = default;

    static Result<Config> load();
    static Result<Config> load_from_path(const std::filesystem::path& path);
    static Result<Config> load_from_default_location();

    std::optional<std::string_view> keypair_path() const noexcept;
    std::string_view rpc_url_or_default() const noexcept;
    Result<std::string> default_active_account_name() const;

private:
    mutable std::string cached_rpc_url_;
};

class ConfigBuilder {
public:
    ConfigBuilder& auto_pay(bool value) {
        config_.auto_pay = value;
        return *this;
    }

    ConfigBuilder& keypair(std::string value) {
        config_.keypair = std::move(value);
        return *this;
    }

    ConfigBuilder& rpc_url(std::string value) {
        config_.rpc_url = std::move(value);
        return *this;
    }

    ConfigBuilder& log_format(LogFormat value) {
        config_.log_format = value;
        return *this;
    }

    Config build() { return std::move(config_); }

private:
    Config config_;
};

inline ConfigBuilder make_config() {
    return ConfigBuilder{};
}

} // namespace dashpay
