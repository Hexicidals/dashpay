#pragma once

#include <cstdint>
#include <string>

namespace dashpay {

class Encoding {
public:
    [[nodiscard]] static std::string percent_encode(std::string_view input);
    [[nodiscard]] static std::string percent_decode(std::string_view input);

    [[nodiscard]] static std::string base64_encode(
        const uint8_t* data, size_t length
    );

    [[nodiscard]] static std::vector<uint8_t> base64_decode(std::string_view input);

    [[nodiscard]] static std::string utf8_encode(std::string_view input);

private:
    static char hex_digit(uint8_t value) {
        return value < 10 ? static_cast<char>('0' + value)
                          : static_cast<char>('a' + value - 10);
    }
};

} // namespace dashpay
