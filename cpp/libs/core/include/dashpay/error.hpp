#pragma once

#include <expected>
#include <format>
#include <string>
#include <variant>

namespace dashpay {

enum class ErrorCode {
    // Config errors
    ConfigNotFound,
    ConfigParseError,
    ConfigValidationError,

    // Network errors
    NetworkError,
    Timeout,
    HttpError,

    // Payment errors
    PaymentFailed,
    InsufficientBalance,
    InvalidTransaction,
    TransactionRejected,
    PaymentProtocolError,

    // Keystore errors
    KeystoreAccessDenied,
    KeystoreLocked,
    KeystoreNotFound,
    KeyNotFound,

    // Solana errors
    SolanaRpcError,
    InvalidPublicKey,
    InvalidSignature,

    // Validation errors
    InvalidAmount,
    InvalidReference,
    InvalidRecipient,

    // Server errors
    ServerError,
    ApiError,

    // MPP/x402 errors
    MppError,
    X402Error,
    UnsupportedPaymentProtocol,

    // Generic errors
    UnknownError,
};

class Error {
public:
    Error() = default;

    explicit Error(ErrorCode code)
        : code_(code) {}

    Error(ErrorCode code, std::string message)
        : code_(code), message_(std::move(message)) {}

    template <typename... Args>
    Error(ErrorCode code, std::format_string<Args...> fmt, Args&&... args)
        : code_(code), message_(std::vformat(fmt.get(), std::make_format_args(args...))) {}

    [[nodiscard]] ErrorCode code() const noexcept { return code_; }

    [[nodiscard]] const std::string& message() const noexcept { return message_; }

    [[nodiscard]] std::string to_string() const {
        if (message_.empty()) {
            return std::format("Error: {}", to_string(code_));
        }
        return std::format("Error: {} - {}", to_string(code_), message_);
    }

private:
    [[nodiscard]] static std::string_view to_string(ErrorCode code) {
        switch (code) {
            case ErrorCode::ConfigNotFound: return "ConfigNotFound";
            case ErrorCode::ConfigParseError: return "ConfigParseError";
            case ErrorCode::ConfigValidationError: return "ConfigValidationError";
            case ErrorCode::NetworkError: return "NetworkError";
            case ErrorCode::Timeout: return "Timeout";
            case ErrorCode::HttpError: return "HttpError";
            case ErrorCode::PaymentFailed: return "PaymentFailed";
            case ErrorCode::InsufficientBalance: return "InsufficientBalance";
            case ErrorCode::InvalidTransaction: return "InvalidTransaction";
            case ErrorCode::TransactionRejected: return "TransactionRejected";
            case ErrorCode::PaymentProtocolError: return "PaymentProtocolError";
            case ErrorCode::KeystoreAccessDenied: return "KeystoreAccessDenied";
            case ErrorCode::KeystoreLocked: return "KeystoreLocked";
            case ErrorCode::KeystoreNotFound: return "KeystoreNotFound";
            case ErrorCode::KeyNotFound: return "KeyNotFound";
            case ErrorCode::SolanaRpcError: return "SolanaRpcError";
            case ErrorCode::InvalidPublicKey: return "InvalidPublicKey";
            case ErrorCode::InvalidSignature: return "InvalidSignature";
            case ErrorCode::InvalidAmount: return "InvalidAmount";
            case ErrorCode::InvalidReference: return "InvalidReference";
            case ErrorCode::InvalidRecipient: return "InvalidRecipient";
            case ErrorCode::ServerError: return "ServerError";
            case ErrorCode::ApiError: return "ApiError";
            case ErrorCode::MppError: return "MppError";
            case ErrorCode::X402Error: return "X402Error";
            case ErrorCode::UnsupportedPaymentProtocol: return "UnsupportedPaymentProtocol";
            case ErrorCode::UnknownError: return "UnknownError";
        }
        return "Unknown";
    }

    ErrorCode code_{ErrorCode::UnknownError};
    std::string message_;
};

template <typename T>
using Result = std::expected<T, Error>;

template <typename T>
using Expected = std::expected<T, Error>;

} // namespace dashpay
