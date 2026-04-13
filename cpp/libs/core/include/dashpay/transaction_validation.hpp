#pragma once

#include <format>
#include <optional>
#include <string>

#include "dashpay/error.hpp"
#include "dashpay/types.hpp"

namespace dashpay::validation {

class TransactionAmountValidator {
public:
    [[nodiscard]] static Result<uint64_t> parse_amount(std::string_view amount_str);

    [[nodiscard]] static Result<double> parse_dash_amount(std::string_view amount_str);

    [[nodiscard]] static std::string format_lamports(uint64_t lamports);

    [[nodiscard]] static std::string format_dash(double dash);
};

} // namespace dashpay::validation
