#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "dashpay/error.hpp"
#include "dashpay/types.hpp"

namespace dashpay {

// x402 Payment Protocol
// Based on: https://x402.org/

struct X402Challenge {
    std::string header;           // "x402-payment" or similar
    std::string challenge;         // Base64 encoded challenge
    std::string network;         // "mainnet", "devnet", etc.
    std::string currency;         // "USDC", "PYUSD", etc.
    uint64_t amount;            // Amount in minor units
    std::string recipient;         // Payment recipient
    std::optional<std::string> reference;   // Transaction reference
};

struct X402Payment {
    std::string signature;        // Signed challenge
    std::string proof;           // Payment proof
    std::string transaction_id;   // Transaction signature
};

class X402Client {
public:
    explicit X402Client(std::string_view rpc_url);

    [[nodiscard]] Result<X402Challenge> parse_challenge(
        const std::vector<std::pair<std::string, std::string>>& headers
    );

    [[nodiscard]] Result<std::string> sign_challenge(
        const X402Challenge& challenge,
        const SecretKey& secret_key
    );

    [[nodiscard]] Result<X402Payment> submit_payment(
        const X402Challenge& challenge,
        const X402Payment& payment
    );

    [[nodiscard]] static bool is_x402_response(
        int status_code,
        const std::vector<std::pair<std::string, std::string>>& headers
    );

private:
    std::string rpc_url_;
};

} // namespace dashpay
