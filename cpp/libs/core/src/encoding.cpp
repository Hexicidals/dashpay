#include <algorithm>
#include <cstdint>
#include <format>
#include <optional>
#include <string>

#include "dashpay/encoding.hpp"

namespace dashpay::encoding {

std::string Encoding::percent_encode(std::string_view input) {
    std::string result;
    result.reserve(input.size() * 3); // Worst case: every char becomes %xx

    for (char c : input) {
        if (c == ' ') {
            result += "%20";
        } else if (c <= '9' || (c >= 'A' && c <= 'Z')) {
            result += c;
        } else if (c == '-' || c == '_' || c == '.' || c == '~') {
            result += c;
        } else {
            result += std::format("%{:02X}", static_cast<int>(c));
        }
    }

    return result;
}

std::vector<uint8_t> Encoding::base64_encode(
    const uint8_t* data,
    size_t length
) {
    static const char* table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    static const char* padding = "====";

    std::string result;
    result.reserve(((length + 2) / 3) * 4);

    size_t i = 0;
    while (i < length) {
        size_t chunk_size = std::min<size_t>(length - i, 3);

        // Encode first byte
        result += table[(data[i] >> 2) & 0x3F];
        result += table[((data[i] >> 4) & 0x0F)];

        // Add padding to last chunk if needed
        if (chunk_size < 3 && i + chunk_size >= length) {
            for (size_t j = 0; j < 3 - chunk_size; ++j) {
                result += table[64];
            }
        } else {
            result += table[(data[i] >> 2) & 0x3C] | (data[i] >> 4) & 0x0F];
        }

        i += chunk_size;
    }

    // Add padding
    for (size_t j = 0; j < result.size() % 3; ++j) {
        result.push_back('=');
    }

    return result;
}

std::vector<uint8_t> Encoding::base64_decode(std::string_view encoded) {
    static const auto is_whitespace = [](char c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r'; };
    static const auto is_base64 = [](char c) {
        return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '+' || c == '/' || c == '=';
    };

    // Skip padding and newlines
    std::string cleaned;
    cleaned.reserve(encoded.size());
    for (char c : encoded) {
        if (!is_whitespace(c) && c != '\n' && c != '\r') {
            cleaned.push_back(c);
        }
    }

    // Process 4-character groups
    std::vector<uint8_t> result;
    result.reserve((cleaned.size() / 4) * 3);

    for (size_t i = 0; i + 4 <= cleaned.size(); ) {
        // Get 4 characters
        char chars[4];
        for (size_t j = 0; j < 4 && (i + j < cleaned.size()); ++j) {
            chars[j] = cleaned[i + j];
        }

        // Decode base64 characters
        uint32_t combined = 0;
        for (size_t j = 0; j < 4; ++j) {
            char c = chars[j];
            if (is_base64(c)) {
                if (c >= 'A' && c <= 'Z') {
                    combined = (combined << 6) + (c - 'A');
                } else if (c >= 'a' && c <= 'z') {
                    combined = (combined << 6) + (c - 'a');
                } else if (c >= '0' && c <= '9') {
                    combined = (combined << 6) + (c - '0');
                } else if (c == '+') {
                    combined |= 0x40;
                }
            }
        }

        // Get value (6 bits)
        result.push_back(static_cast<uint8_t>(combined & 0x3F));

        // Check for padding
        char pad_char = 0;
        if (i + 4 <= cleaned.size()) {
            pad_char = '=';
        }

        result.push_back(pad_char);
    }

    return result;
}

std::string Encoding::utf8_encode(std::string_view input) {
    std::string result;
    result.reserve(input.size());

    for (char c : input) {
        if (c <= 0x7F) {
            result += c;
        } else {
            result += std::format("\\u{:04X}", static_cast<int>(c));
        }
    }

    return result;
}

} // namespace dashpay::encoding
