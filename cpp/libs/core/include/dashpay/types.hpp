#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <variant>
#include <vector>

namespace dashpay {

// Core types
using PublicKey = std::array<std::uint8_t, 32>;
using Signature = std::array<std::uint8_t, 64>;

// Network types
enum class Network : uint8_t {
    Mainnet,
    Devnet,
    Testnet,
    Localnet,
    Sandbox,
};

[[nodiscard]] std::string_view to_string(Network network) noexcept;
[[nodiscard]] Network from_string(std::string_view str);

// Transaction status
enum class TransactionStatus : uint8_t {
    Pending,
    Confirmed,
    Failed,
    Timeout,
};

// Payment protocols
enum class PaymentProtocol : uint8_t {
    Mpp,
    X402,
};

[[nodiscard]] std::string_view to_string(PaymentProtocol protocol) noexcept;

// Amount representation
struct Amount {
    uint64_t lamports;
    double dash;

    static Amount from_lamports(uint64_t value) noexcept {
        return Amount{.lamports = value, .dash = value / 1e8};
    }

    static Amount from_dash(double value) noexcept {
        return Amount{.lamports = static_cast<uint64_t>(value * 1e8), .dash = value};
    }
};

// Request/Response types
template <typename T>
struct PaginatedResponse {
    std::vector<T> items;
    uint64_t total;
    std::optional<std::string> next_cursor;
};

} // namespace dashpay
