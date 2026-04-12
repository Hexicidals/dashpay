#include <algorithm>
#include <cctype>
#include <charconv>
#include <cstdint>
#include <string>
#include <string_view>

#include "dashpay/types.hpp"
#include "dashpay/types/util.hpp"

namespace dashpay::util {

std::string StringUtil::trim(std::string_view str) {
    const char* whitespace = " \t\n\r";
    const auto start = str.find_first_not_of(whitespace);
    if (start == std::string_view::npos) {
        return std::string();
    }
    const auto end = str.find_last_not_of(whitespace);
    return std::string(str.substr(start, end - start + 1));
}

std::string StringUtil::trim_left(std::string_view str) {
    const char* whitespace = " \t\n\r";
    const auto start = str.find_first_not_of(whitespace);
    if (start == std::string_view::npos) {
        return std::string();
    }
    return std::string(str.substr(start));
}

std::string StringUtil::trim_right(std::string_view str) {
    const char* whitespace = " \t\n\r";
    const auto end = str.find_last_not_of(whitespace);
    if (end == std::string_view::npos) {
        return std::string(str);
    }
    return std::string(str.substr(0, end + 1));
}

bool StringUtil::starts_with(std::string_view str, std::string_view prefix) noexcept {
    return str.size() >= prefix.size() &&
           str.compare(0, prefix.size(), prefix) == 0;
}

bool StringUtil::ends_with(std::string_view str, std::string_view suffix) noexcept {
    return str.size() >= suffix.size() &&
           str.compare(str.size() - suffix.size(), suffix) == 0;
}

std::vector<std::string> StringUtil::split(
    std::string_view str,
    std::string_view delimiter,
    size_t max_splits
) {
    std::vector<std::string> result;
    size_t start = 0;
    size_t end = str.find(delimiter);

    while (end != std::string_view::npos && (max_splits == 0 || result.size() < max_splits - 1)) {
        result.push_back(std::string(str.substr(start, end - start)));
        start = end + delimiter.size();
        end = str.find(delimiter, start);

        if (max_splits > 0 && result.size() >= max_splits) {
            // Add the rest as the last element
            result.push_back(std::string(str.substr(start)));
            break;
        }
    }

    // Add the remaining part if it exists
    if (start < str.size()) {
        result.push_back(std::string(str.substr(start)));
    }

    return result;
}

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

bool StringUtil::equals_ignore_case(
    std::string_view a,
    std::string_view b
) noexcept {
    if (a.size() != b.size()) {
        return false;
    }
    return std::equal(
        a.begin(), a.end(),
        b.begin(), b.end(),
        [](char x, char y) {
            return std::tolower(static_cast<unsigned char>(x)) ==
                   std::tolower(static_cast<unsigned char>(y));
        }
    );
}

std::string HexUtil::encode(const uint8_t* data, size_t length) {
    const char* hex_chars = "0123456789abcdef";
    std::string result;
    result.reserve(length * 2);

    for (size_t i = 0; i < length; ++i) {
        result.push_back(hex_chars[data[i] >> 4]);
        result.push_back(hex_chars[data[i] & 0x0F]);
    }

    return result;
}

std::vector<uint8_t> HexUtil::decode(std::string_view hex) {
    std::vector<uint8_t> result;
    result.reserve(hex.size() / 2);

    for (size_t i = 0; i + 1 < hex.size(); i += 2) {
        uint8_t high = 0;
        uint8_t low = 0;

        if (hex[i] >= '0' && hex[i] <= '9') {
            high = hex[i] - '0';
        } else if (hex[i] >= 'a' && hex[i] <= 'f') {
            high = hex[i] - 'a' + 10;
        }

        if (hex[i + 1] >= '0' && hex[i + 1] <= '9') {
            low = hex[i + 1] - '0';
        } else if (hex[i + 1] >= 'a' && hex[i + 1] <= 'f') {
            low = hex[i + 1] - 'a' + 10;
        }

        result.push_back((high << 4) | low);
    }

    return result;
}

bool HexUtil::is_valid(std::string_view hex) noexcept {
    if (hex.empty() || hex.size() % 2 != 0) {
        return false;
    }

    const char* valid_chars = "0123456789abcdefABCDEF";
    for (char c : hex) {
        if (std::find(valid_chars, c) == std::string_view::npos) {
            return false;
        }
    }
    return true;
}

std::string AmountUtil::format_lamports(uint64_t lamports) {
    return std::format("{}", lamports);
}

std::string AmountUtil::format_dash(double amount, uint8_t decimals) {
    const uint64_t multiplier = 1;
    for (uint8_t i = 0; i < decimals; ++i) {
        multiplier *= 10;
    }
    return std::format("{:.{}f}", amount, decimals);
}

Result<uint64_t> AmountUtil::parse_lamports(std::string_view str) {
    uint64_t result;
    if (auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), result);
        ec != std::errc()) {
        return Error(ErrorCode::InvalidAmount,
            "Invalid lamports value: {}"_format(str));
    }
    return result;
}

Result<double> AmountUtil::parse_dash(std::string_view str) {
    // Find decimal point
    size_t dot_pos = str.find('.');
    uint8_t decimals = 9; // Default for Dash

    if (dot_pos != std::string_view::npos) {
        decimals = static_cast<uint8_t>(str.size() - dot_pos - 1);
    }

    double multiplier = 1.0;
    for (uint8_t i = 0; i < decimals; ++i) {
        multiplier /= 10.0;
    }

    // Parse integer part
    std::string int_part = (dot_pos == std::string_view::npos)
        ? std::string(str)
        : std::string(str.substr(0, dot_pos));

    uint64_t int_value;
    if (auto [ptr, ec] = std::from_chars(int_part.data(), int_part.data() + int_part.size(), int_value);
        ec != std::errc()) {
        return Error(ErrorCode::InvalidAmount, "Invalid amount value");
    }

    // Parse decimal part
    if (dot_pos != std::string_view::npos) {
        std::string dec_part = str.substr(dot_pos + 1);
        double dec_value = 0.0;
        double dec_divisor = 1.0;

        for (char c : dec_part) {
            if (c >= '0' && c <= '9') {
                dec_value = dec_value * 10 + (c - '0');
            } else if (c >= 'A' && c <= 'F') {
                dec_value = dec_value * 10 + (c - 'A' + 10);
            }
            dec_divisor *= 10;
        }

        int_value += static_cast<uint64_t>(dec_value / dec_divisor);
    }

    return int_value * multiplier;
}

} // namespace dashpay::util
