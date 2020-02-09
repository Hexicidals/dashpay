#include <dashpay/transaction_validation.hpp>
#include <algorithm>
#include <charconv>
#include <cctype>

namespace dashpay::validation {

Result<uint64_t> TransactionAmountValidator::parse_amount(std::string_view amount_str) {
    if (amount_str.empty()) {
        return Error(ErrorCode::InvalidAmount, "Amount cannot be empty");
    }

    // Trim whitespace
    std::string trimmed;
    trimmed.reserve(amount_str.size());
    for (char c : amount_str) {
        if (!std::isspace(static_cast<unsigned char>(c))) {
            trimmed += c;
        }
    }

    // Check for negative amounts
    if (trimmed[0] == '-') {
        return Error(ErrorCode::InvalidAmount, "Amount cannot be negative");
    }

    // Parse as integer (lamports)
    uint64_t result = 0;
    for (char c : trimmed) {
        if (std::isdigit(static_cast<unsigned char>(c))) {
            result = result * 10 + (c - '0');
        }
    }

    if (result == 0) {
        return Error(ErrorCode::InvalidAmount,
                    std::format("Failed to parse amount: '{}'", amount_str));
    }

    // Reasonable bounds: 1 lamport to 1 billion lamports
    if (result < 1) {
        return Error(ErrorCode::InvalidAmount,
                    "Amount must be at least 1 lamport");
    }

    if (result > 100000000000000) {
        return Error(ErrorCode::InvalidAmount,
                    "Amount cannot exceed 1 billion lamports");
    }

    return result;
}

Result<double> TransactionAmountValidator::parse_dash_amount(std::string_view amount_str) {
    if (amount_str.empty()) {
        return Error(ErrorCode::InvalidAmount, "Amount cannot be empty");
    }

    // Remove spaces
    std::string trimmed;
    trimmed.reserve(amount_str.size());
    for (char c : amount_str) {
        if (c != ' ') {
            trimmed += c;
        }
    }

    double result = 0.0;
    try {
        size_t pos = 0;
        bool decimal_found = false;

        for (size_t i = 0; i < trimmed.size(); ++i) {
            char c = trimmed[i];

            if (c == '.') {
                if (decimal_found) {
                    return Error(ErrorCode::InvalidAmount,
                                "Amount has multiple decimal points");
                }
                decimal_found = true;
                pos = i;
            } else if (std::isdigit(static_cast<unsigned char>(c))) {
                if (decimal_found) {
                    result += static_cast<double>(c - '0') / std::pow(10.0, (trimmed.size() - i - pos));
                } else {
                    result = result * 10 + static_cast<double>(c - '0');
                }
                pos = i;
            }
        }

        if (result < 0) {
            return Error(ErrorCode::InvalidAmount, "Amount cannot be negative");
        }

        if (result > 1000000000.0) {
            return Error(ErrorCode::InvalidAmount, "Amount cannot exceed 1 billion DASH");
        }

    } catch (...) {
        return Error(ErrorCode::InvalidAmount, "Invalid amount format");
    }

    return result;
}

std::string TransactionAmountValidator::format_lamports(uint64_t lamports) {
    return std::format("{} lamports", lamports);
}

std::string TransactionAmountValidator::format_dash(double amount) {
    if (amount == static_cast<int64_t>(amount)) {
        return std::format("{} DASH", static_cast<int64_t>(amount));
    }

    // Format with appropriate decimal places (typically 8 for DASH)
    return std::format("{:.8f} DASH", amount);
}

} // namespace dashpay::validation
