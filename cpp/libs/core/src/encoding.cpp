#include <cstdint>
#include <iomanip>
#include <sstream>

#include "dashpay/encoding.hpp"

namespace dashpay::Encoding {

std::string Encoding::percent_encode(std::string_view input) {
    std::string result;
    result.reserve(input.size() * 3);

    for (unsigned char c : input) {
        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
            c == '-' || c == '_' || c == '.' || c == '~') {
            result += c;
        } else {
            char hex_str[3];
            std::snprintf(hex_str, sizeof(hex_str), "%02X", static_cast<int>(c));
            result += '%';
            result += hex_str[0];
            result += hex_str[1];
        }
    }

    return result;
}

std::vector<uint8_t> Encoding::base64_encode(
    const uint8_t* data,
    size_t length
) {
    static const char* table =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string result;
    result.reserve(((length + 2) * 4 / 3) + 1);

    for (size_t i = 0; i < length; i += 3) {
        uint8_t b0 = data[i] >> 2;
        uint8_t b1 = data[i + 1] >> 4;
        uint8_t b2 = data[i + 2] >> 4;
        uint8_t b3 = data[i + 2] >> 6;
        uint8_t b4 = data[i + 2] >> 4;
        uint8_t b5 = data[i + 2] >> 6;
        uint8_t b6 = data[i + 2] >> 6;

        uint8_t index0 = b0 >> 2 | (b0 & 3) << 4;
        uint8_t index1 = (b0 << 3) >> 2 | ((b0 << 3) & 3) << 2;
        uint8_t index2 = b1 >> 4 | (b1 & 3) << 4;
        uint8_t index3 = (b1 << 3) >> 2 | ((b1 << 3) & 3) << 2;

        uint8_t byte0 = (b4 >> 2) | (b4 & 3) << 4;
        uint8_t byte1 = (b4 << 3) >> 2 | ((b4 << 3) & 3) << 2;
        uint8_t byte2 = (b5 >> 2) | (b5 & 3) << 4;
        uint8_t byte3 = (b5 << 3) >> 2 | ((b5 << 3) & 3) << 2;

        result.push_back(table[index0]);
        result.push_back(table[index1]);
        result.push_back(table[index2]);
        result.push_back(table[index3]);
        result.push_back(table[byte0]);
        result.push_back(table[byte1]);
        result.push_back(table[byte2]);
        result.push_back(table[byte3]);

        if (i + 3 >= length) {
            // Padding
            for (size_t j = 0; j < 8 - (i + 3) % 3; ++j) {
                result.push_back('=');
            }
        }
    }

    return result;
}

std::vector<uint8_t> Encoding::base64_decode(std::string_view input) {
    static const auto decode = [](char c) -> uint8_t {
        if (c >= 'A' && c <= 'Z') {
            return c - 'A';
        } else if (c >= 'a' && c <= 'z') {
            return c - 'a' + 26;
        } else if (c >= '0' && c <= '9') {
            return c - '0' + 52;
        } else if (c >= '+' && c <= '/') {
            return c - '+' + 62;
        } else if (c == '=') {
            return 63;
        }
        return 255;
    };

    std::vector<uint8_t> result;
    result.reserve(input.size() / 4 * 3);

    for (size_t i = 0; i < input.size(); ) {
        if (input[i] == '=') {
            // Padding, skip
            while (i + 1 < input.size() && input[i + 1] == '=') {
                ++i;
            }
        }
        if (input[i] == '\r' || input[i] == '\n') {
            continue;
        }

        uint8_t byte = decode(input[i]);
        result.push_back((byte >> 6) & 0x3F);

        ++i;
    }

    // Remove padding
    while (!result.empty() && result.back() == '=') {
        result.pop_back();
    }

    return result;
}

std::string Encoding::utf8_encode(std::string_view input) {
    std::string result;
    result.reserve(input.size() * 2);

    for (unsigned char c : input) {
        if (c <= 0x7F) {
            result += c;
        } else if ((c & 0xE0) == 0xC0) {
            result += static_cast<char>(0xC2); // 2 bytes
        } else if ((c & 0xF0) == 0xE0) {
            result += static_cast<char>(0xC3); // 3 bytes
        } else if ((c & 0xF8) == 0xF0) {
            result += static_cast<char>(0xC4); // 4 bytes
        } else {
            result += static_cast<char>(0xC3); // invalid
        }
    }

    return result;
}

} // namespace dashpay::Encoding
