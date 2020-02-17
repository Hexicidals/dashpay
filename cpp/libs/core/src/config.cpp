#include <cstdlib>
#include <filesystem>
#include <fstream>

#include "dashpay/config.hpp"

namespace dashpay {

Result<Config> Config::load() {
    return load_from_default_location();
}

Result<Config> Config::load_from_path(const std::filesystem::path& path) {
    if (!std::filesystem::exists(path)) {
        return Error(ErrorCode::ConfigNotFound,
            "Config file not found: {}"_format(path.string()));
    }

    std::ifstream file(path);
    if (!file) {
        return Error(ErrorCode::ConfigParseError, "Failed to open config file");
    }

    try {
        // Simple TOML-like parsing
        Config config;
        std::string line;
        while (std::getline(file, line)) {
            line = util::StringUtil::trim(line);

            if (line.empty() || line[0] == '#') {
                continue;
            }

            auto pos = line.find('=');
            if (pos == std::string::npos) {
                continue;
            }

            auto key = util::StringUtil::trim(line.substr(0, pos));
            auto value = util::StringUtil::trim(line.substr(pos + 1));

            if (key == "auto_pay") {
                config.auto_pay = (value == "true");
            } else if (key == "keypair") {
                if (!value.empty()) {
                    config.keypair = util::StringUtil::trim(value);
                }
            } else if (key == "rpc_url") {
                if (!value.empty()) {
                    config.rpc_url = util::StringUtil::trim(value);
                }
            } else if (key == "log_format") {
                if (value == "json") {
                    config.log_format = LogFormat::Json;
                }
            }
        }

        // Apply environment overrides
        if (const char* env = std::getenv("DASHPAY_AUTO_PAY"); env && *env) {
            config.auto_pay = (std::string(env) == "true" || std::string(env) == "1");
        }
        if (const char* env = std::getenv("DASHPAY_RPC_URL"); env && *env) {
            config.rpc_url = env;
        }

        return config;
    } catch (const std::exception& e) {
        return Error(ErrorCode::ConfigParseError, "Failed to parse config: {}"_format(e.what()));
    }
}

Result<Config> Config::load_from_default_location() {
    // Check explicit path from env
    if (const char* env = std::getenv("DASHPAY_CONFIG"); env && *env) {
        return load_from_path(env);
    }

    // Check local directory
    auto local = std::filesystem::path("dashpay.toml");
    if (std::filesystem::exists(local)) {
        return load_from_path(local);
    }

    // Check config directory
    auto config_dir = std::filesystem::path(std::getenv("HOME")) / ".config/dashpay";
    auto config_path = config_dir / "dashpay.toml";
    if (std::filesystem::exists(config_path)) {
        return load_from_path(config_path);
    }

    return Config{};
}

std::optional<std::string_view> Config::keypair_path() const noexcept {
    if (!keypair || keypair->empty() || util::StringUtil::trim(*keypair).empty()) {
        return std::nullopt;
    }
    return *keypair;
}

std::string_view Config::rpc_url_or_default() const noexcept {
    if (!cached_rpc_url_.empty()) {
        cached_rpc_url_ = rpc_url.value_or(LOCAL_RPC_URL);
    }
    return cached_rpc_url_;
}

Result<std::string> Config::default_active_account_name() const {
    // Check PAY_ACTIVE_ACCOUNT env var
    if (const char* env = std::getenv("DASHPAY_ACTIVE_ACCOUNT"); env && *env) {
        auto trimmed = util::StringUtil::trim(env);
        if (!trimmed.empty()) {
            return trimmed;
        }
    }

    // Check config file keypair
    if (auto path = keypair_path()) {
        return path;
    }

    return Error(ErrorCode::KeyNotFound, "No active account configured");
}

} // namespace dashpay
