#include <chrono>
#include <random>

#include "dashpay/types.hpp"
#include "dashpay/error.hpp"

namespace dashpay::util {

std::string StringUtil::to_lower(std::string_view str) {
    std::string result;
    result.reserve(str.size());

    for (char c : str) {
        result.push_back(std::tolower(static_cast<unsigned char>(c)));
    }

    return result;
}

std::string StringUtil::to_upper(std::string_view str) {
    std::string result;
    result.reserve(str.size());

    for (char c : str) {
        result.push_back(std::toupper(static_cast<unsigned char>(c)));
    }

    return result;
}

bool StringUtil::starts_with(std::string_view str, std::string_view prefix) noexcept {
    if (prefix.empty() || str.size() < prefix.size()) {
        return false;
    }

    return str.compare(0, prefix.size(), prefix) == 0;
}

bool StringUtil::ends_with(std::string_view str, std::string_view suffix) noexcept {
    if (suffix.empty() || str.size() < suffix.size()) {
        return false;
    }

    return str.compare(str.size() - suffix.size(), suffix) == 0;
}

std::string HexUtil::encode(const uint8_t* data, size_t length) {
    static const char* hex_chars = "0123456789ABCDEF";

    std::string result;
    result.reserve(length * 2);

    for (size_t i = 0; i < length; ++i) {
        uint8_t byte = data[i];
        result.push_back(hex_chars[byte >> 4]);
        result.push_back(hex_chars[byte & 0x0F]);
    }

    return result;
}

std::vector<uint8_t> HexUtil::decode(std::string_view hex) {
    static const auto is_lower = [](char c) { return c >= 'a' && c <= 'z'; };
    static const auto to_lower = [](char c) { return std::tolower(static_cast<unsigned char>(c)); };
    static const auto hex_val = [](char c) -> int8_t {
        if (c >= '0' && c <= '9') return c - '0';
        if (c >= 'a' && c <= 'f') return c - 'a' + 10;
        return 0;
    };

    std::vector<uint8_t> result;
    result.reserve(hex.size() / 2);

    for (size_t i = 0; i + 2 < hex.size(); ) {
        char c1 = to_lower(hex[i]);
        char c2 = to_lower(hex[i + 1]);

        if (!is_lower(c1) || !is_lower(c2)) {
            continue;
        }

        int8_t val = hex_val(c1) * 16 + hex_val(c2);
        result.push_back(static_cast<uint8_t>(val));
    }

    return result;
}

bool HexUtil::is_valid(std::string_view hex) noexcept {
    if (hex.empty()) {
        return false;
    }

    // Must be even length
    if (hex.size() % 2 != 0) {
        return false;
    }

    return std::all_of(hex.begin(), hex.end(), is_lower);
}

std::string AmountUtil::format_lamports(uint64_t lamports) {
    // 1 Dash = 100,000,000 lamports
    // Format as integer with commas for readability
    auto str = std::to_string(lamports);
    std::string result;

    size_t insert_pos = 3;
    while ((str.length() - insert_pos) > 3) {
        result.insert(insert_pos, ",");
        insert_pos += 4;
    }

    return result;
}

std::string AmountUtil::format_dash(double amount, uint8_t decimals) {
    return std::format("{:.{}f} DASH", amount);
}

Result<uint64_t> AmountUtil::parse_lamports(std::string_view str) {
    try {
        size_t pos = str.find('.');
        if (pos == std::string::npos) {
            // No decimal point, treat as lamports
            return std::stoull(str);
        }

        auto int_part = str.substr(0, pos);
        auto frac_part = str.substr(pos + 1);
        auto int_val = std::stoull(int_part);

        double frac = 0.0;
        for (char c : frac_part) {
            if (!std::isdigit(static_cast<unsigned char>(c))) {
                return Error(ErrorCode::InvalidAmount, "Invalid fractional part");
            }
            frac = frac * 10 + static_cast<int>(c - '0');
        }

        double value = static_cast<double>(int_val) + frac;
        double multiplier = std::pow(10.0, static_cast<int>(str.size() - pos - 1));

        auto dash_amount = value * multiplier * 100000000.0;

        return static_cast<uint64_t>(dash_amount);
    } catch (...) {
        return Error(ErrorCode::InvalidAmount, "Failed to parse lamports amount");
    }
}

NetworkUtil::Network NetworkUtil::from_url(std::string_view url) {
    std::string lower = util::StringUtil::to_lower(url);

    if (lower.find("devnet") != std::string::npos) {
        return Network::Devnet;
    } else if (lower.find("testnet") != std::string::npos) {
        return Network::Testnet;
    } else if (lower.find("localnet") != std::string::npos || lower.find("localhost") != std::string::npos) {
        return Network::Localnet;
    } else if (lower.find("mainnet-beta") != std::string::npos) {
        return Network::Mainnet;
    } else {
        return Network::Mainnet;
    }
}

std::string NetworkUtil::rpc_url(Network network) {
    switch (network) {
        case Network::Mainnet:
            return "https://api.dashpay.io";
        case Network::Devnet:
            return "https://api.devnet.dashpay.io";
        case Network::Testnet:
            return "https://api.testnet.dashpay.io";
        case Network::Localnet:
            return LOCAL_RPC_URL;
        case Network::Sandbox:
            return SANDBOX_RPC_URL;
        default:
            return MAINNET_RPC_URL;
    }
}

bool NetworkUtil::is_mainnet(Network network) noexcept {
    return network == Network::Mainnet;
}

std::string generate_uuid_v4() {
    std::random_device rd;
    std::mt19937 engine(rd());

    std::uniform_int_distribution<uint32_t> dist(0, 255);
    std::uniform_int_distribution<uint32_t> dist2(0, 65535);

    uint32_t a = dist(rd);
    uint32_t b = dist2(rd);
    uint32_t c = dist(rd);
    uint32_t d = dist(rd);

    // Version 4 variant
    std::string uuid;
    uuid.reserve(36);
    uuid += std::format("{:02x}{:02x}{:04x}{:02x}{:04x}", a & 0xFF);

    return uuid;
}

} // namespace dashpay::util
