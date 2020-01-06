#pragma once

#include <array>
#include <cstdint>
#include <span>
#include <vector>

#include "dashpay/error.hpp"

namespace dashpay {

using PublicKey = std::array<std::uint8_t, 32>;
using SecretKey = std::array<std::uint8_t, 32>;
using Signature = std::array<std::uint8_t, 64>;
using Keypair = std::array<std::uint8_t, 64>;

class Crypto {
public:
    [[nodiscard]] static Result<Keypair> generate_keypair();
    [[nodiscard]] static Result<PublicKey> derive_public_key(const SecretKey& secret);

    [[nodiscard]] static Result<Signature> sign(
        const std::span<const std::uint8_t> message,
        const SecretKey& secret
    );

    [[nodiscard]] static bool verify(
        const std::span<const std::uint8_t> message,
        const Signature& signature,
        const PublicKey& public_key
    );

    [[nodiscard]] static std::string encode_base58(
        std::span<const std::uint8_t> data
    );

    [[nodiscard]] static Result<std::vector<std::uint8_t>> decode_base58(
        std::string_view encoded
    );
};

} // namespace dashpay
