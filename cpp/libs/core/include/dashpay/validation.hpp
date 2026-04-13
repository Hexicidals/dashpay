#pragma once

#include <cstdint>
#include <expected>
#include <string_view>

#include "dashpay/error.hpp"

namespace dashpay {

class TransactionValidator {
public:
    [[nodiscard]] static Result<void> validate_amount(uint64_t lamports);

    [[nodiscard]] static Result<void> validate_recipient(std::string_view pubkey);

    [[nodiscard]] static Result<void> validate_reference(std::string_view reference);

    [[nodiscard]] static Result<void> validate_transaction(
        std::string_view recipient,
        uint64_t amount,
        std::string_view reference
    );

    [[nodiscard]] static bool is_valid_lamports(uint64_t value) noexcept;

    [[nodiscard]] static uint64_t lamports_to_dash(uint64_t lamports) noexcept;
    [[nodiscard]] static uint64_t dash_to_lamports(double dash) noexcept;
};

} // namespace dashpay
