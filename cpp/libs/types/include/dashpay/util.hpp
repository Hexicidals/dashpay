#pragma once

#include <cstdint>
#include <format>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace dashpay::util {

class StringUtil {
public:
    [[nodiscard]] static std::string trim(std::string_view str);
    [[nodiscard]] static std::string trim_left(std::string_view str);
    [[nodiscard]] static std::string trim_right(std::string_view str);

    [[nodiscard]] static bool starts_with(std::string_view str, std::string_view prefix) noexcept;
    [[nodiscard]] static bool ends_with(std::string_view str, std::string_view suffix) noexcept;

    [[nodiscard]] static std::vector<std::string> split(
        std::string_view str,
        std::string_view delimiter,
        size_t max_splits = 0
    );

    [[nodiscard]] static std::string to_lower(std::string_view str);
    [[nodiscard]] static std::string to_upper(std::string_view str);

    [[nodiscard]] static bool equals_ignore_case(
        std::string_view a,
        std::string_view b
    ) noexcept;
};

class HexUtil {
public:
    [[nodiscard]] static std::string encode(const uint8_t* data, size_t length);
    [[nodiscard]] static std::vector<uint8_t> decode(std::string_view hex);
    [[nodiscard]] static bool is_valid(std::string_view hex) noexcept;
};

class AmountUtil {
public:
    [[nodiscard]] static std::string format_lamports(uint64_t lamports);
    [[nodiscard]] static std::string format_dash(double amount, uint8_t decimals = 9);

    [[nodiscard]] static Result<uint64_t> parse_lamports(std::string_view str);
    [[nodiscard]] static Result<double> parse_dash(std::string_view str);
};

class NetworkUtil {
public:
    [[nodiscard]] static Network from_url(std::string_view url);
    [[nodiscard]] static std::string rpc_url(Network network);
    [[nodiscard]] static bool is_mainnet(Network network) noexcept { return network == Network::Mainnet; }
};

} // namespace dashpay::util
