#pragma once

#include <array>
#include <cstdint>
#include <expected>
#include <memory>
#include <span>
#include <string>

#include "dashpay/error.hpp"

namespace dashpay {

class Crypto {
public:
    static Result<std::array<std::uint8_t, 32>> generate_keypair();

    static Result<std::array<std::uint8_t, 32>> derive_public_key(
        const std::array<std::uint8_t, 32>& secret
    );

    static Result<std::array<std::uint8_t, 64>> sign(
        std::span<const std::uint8_t> message,
        const std::array<std::uint8_t, 32>& secret
    );

    static bool verify(
        std::span<const std::uint8_t> message,
        const std::array<std::uint8_t, 64>& signature,
        const std::array<std::uint8_t, 32>& public_key
    ) noexcept;

    static std::string encode_base58(
        std::span<const std::uint8_t> data
    );

    static Result<std::vector<std::uint8_t>> decode_base58(
        std::string_view encoded
    );

    static Result<std::string> encode_base64(
        std::span<const std::uint8_t> data
    );

    static Result<std::vector<std::uint8_t>> decode_base64(
        std::string_view encoded
    );

private:
    // Ed25519 keypair generation (uses OpenSSL)
    static Result<std::array<std::uint8_t, 64>> ed25519_generate_keypair();
};

} // namespace dashpay
