#include <dashpay/crypto.hpp>
#include <openssl/evp.h>
#include <openssl/rand.h>

#include <dashpay/error.hpp>

namespace dashpay {

Result<std::array<std::uint8_t, 64>> Crypto::ed25519_generate_keypair() {
    std::array<std::uint8_t, 64> keypair{};

    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new();
    if (!ctx) {
        return Error(ErrorCode::CryptoError, "Failed to create EVP context");
    }

    EVP_PKEY* pkey = EVP_PKEY_new();
    if (!pkey) {
        EVP_PKEY_CTX_free(ctx);
        return Error(ErrorCode::CryptoError, "Failed to create EVP_PKEY");
    }

    if (EVP_PKEY_set_type(pkey, EVP_PKEY_ED25519) != 1) {
        EVP_PKEY_free(pkey);
        EVP_PKEY_CTX_free(ctx);
        return Error(ErrorCode::CryptoError, "Failed to set Ed25519 key type");
    }

    if (EVP_PKEY_generate_key(pkey) != 1) {
        EVP_PKEY_free(pkey);
        EVP_PKEY_CTX_free(ctx);
        return Error(ErrorCode::CryptoError, "Failed to generate Ed25519 key");
    }

    EVP_PKEY* privkey = EVP_PKEY_new();
    EVP_PKEY_get_raw_private_key(ctx, pkey, &privkey);

    if (!privkey) {
        EVP_PKEY_free(pkey);
        EVP_PKEY_CTX_free(ctx);
        return Error(ErrorCode::CryptoError, "Failed to get private key");
    }

    const unsigned char* priv_bytes = nullptr;
    size_t priv_len = 0;
    EVP_PKEY_get_raw_private_key(ctx, pkey, &priv_bytes, &priv_len);

    if (!priv_bytes || priv_len != 32) {
        EVP_PKEY_free(pkey);
        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(privkey);
        return Error(ErrorCode::CryptoError, "Invalid private key length");
    }

    // Copy secret key (32 bytes)
    std::memcpy(keypair.data(), priv_bytes, 32);

    // Get public key
    const unsigned char* pub_bytes = nullptr;
    size_t pub_len = 0;
    EVP_PKEY_get_raw_public_key(ctx, pkey, &pub_bytes, &pub_len);

    if (!pub_bytes || pub_len != 32) {
        EVP_PKEY_free(pkey);
        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(privkey);
        return Error(ErrorCode::CryptoError, "Invalid public key length");
    }

    // Copy public key (32 bytes)
    std::memcpy(keypair.data() + 32, pub_bytes, 32);

    EVP_PKEY_free(pkey);
    EVP_PKEY_CTX_free(ctx);
    EVP_PKEY_free(privkey);

    return keypair;
}

Result<std::array<std::uint8_t, 32>> Crypto::derive_public_key(
    const std::array<std::uint8_t, 64>& keypair
) {
    std::array<std::uint8_t, 32> public_key{};

    // Copy public key portion (last 32 bytes)
    std::memcpy(public_key.data(), keypair.data() + 32, 32);

    return public_key;
}

Result<std::array<std::uint8_t, 64>> Crypto::sign(
    std::span<const std::uint8_t> message,
    const std::array<std::uint8_t, 32>& secret
) {
    std::array<std::uint8_t, 64> signature{};

    EVP_MD_CTX* mdctx = EVP_MD_CTX_new(EVP_sha512());
    if (!mdctx) {
        return Error(ErrorCode::CryptoError, "Failed to create SHA-512 context");
    }

    // Hash the message
    unsigned char hash[EVP_MAX_MD_SIZE];
    EVP_DigestInit_ex(mdctx, nullptr, EVP_sha512(), nullptr, nullptr);
    EVP_DigestUpdate(mdctx, message.data(), message.size());
    EVP_DigestFinal_ex(mdctx, hash, nullptr);
    EVP_MD_CTX_free(mdctx);

    // Sign the hash
    EVP_PKEY* pkey = EVP_PKEY_new_raw_priv_key(
        EVP_PKEY_ED25519,
        secret.data(),
        32
    );

    if (!pkey) {
        EVP_MD_CTX_free(mdctx);
        return Error(ErrorCode::CryptoError, "Failed to create Ed25519 private key");
    }

    EVP_PKEY_CTX* evpctx = EVP_PKEY_CTX_new();
    if (!evpctx) {
        EVP_PKEY_free(pkey);
        EVP_MD_CTX_free(mdctx);
        return Error(ErrorCode::CryptoError, "Failed to create EVP context");
    }

    EVP_PKEY_CTX_init(evpctx, pkey);

    EVP_MD_CTX* mdctx = EVP_MD_CTX_new(EVP_sha512());
    if (!mdctx) {
        EVP_PKEY_CTX_free(evpctx);
        EVP_PKEY_free(pkey);
        return Error(ErrorCode::CryptoError, "Failed to create SHA-512 context");
    }

    EVP_DigestInit_ex(mdctx, nullptr, EVP_sha512(), nullptr, nullptr);
    EVP_DigestUpdate(mdctx, hash, EVP_MAX_MD_SIZE);

    size_t siglen = 0;
    if (EVP_SignFinal(evpctx, signature.data(), &siglen) != 1) {
        EVP_MD_CTX_free(mdctx);
        EVP_PKEY_CTX_free(evpctx);
        EVP_PKEY_free(pkey);
        return Error(ErrorCode::CryptoError, "Failed to sign message");
    }

    EVP_MD_CTX_free(mdctx);
    EVP_PKEY_CTX_free(evpctx);
    EVP_PKEY_free(pkey);

    // Ed25519 signatures are 64 bytes
    if (siglen != 64) {
        return Error(ErrorCode::CryptoError, "Invalid signature length");
    }

    EVP_MD_CTX_free(mdctx);

    return signature;
}

bool Crypto::verify(
    std::span<const std::uint8_t> message,
    const std::array<std::uint8_t, 64>& signature,
    const std::array<std::uint8_t, 32>& public_key
) noexcept {
    // Reconstruct the public key
    EVP_PKEY* pkey = EVP_PKEY_new_raw_pub_key(
        EVP_PKEY_ED25519,
        public_key.data(),
        32
    );

    if (!pkey) {
        return false;
    }

    // Hash the message
    unsigned char hash[EVP_MAX_MD_SIZE];
    EVP_MD_CTX* mdctx = EVP_MD_CTX_new(EVP_sha512());
    if (mdctx) {
        EVP_PKEY_free(pkey);
        return false;
    }

    EVP_DigestInit_ex(mdctx, nullptr, EVP_sha512(), nullptr, nullptr);
    EVP_DigestUpdate(mdctx, message.data(), message.size());
    EVP_DigestFinal_ex(mdctx, hash, nullptr);

    // Verify the signature
    EVP_PKEY_CTX* evpctx = EVP_PKEY_CTX_new();
    if (!evpctx) {
        EVP_MD_CTX_free(mdctx);
        EVP_PKEY_free(pkey);
        return false;
    }

    EVP_PKEY_CTX_init(evpctx, pkey);

    if (EVP_VerifyFinal(evpctx, signature.data(), 64, hash, EVP_MAX_MD_SIZE) != 1) {
        EVP_MD_CTX_free(mdctx);
        EVP_PKEY_CTX_free(evpctx);
        EVP_PKEY_free(pkey);
        return false;
    }

    bool result = (EVP_VerifyFinal(evpctx, signature.data(), 64, hash, EVP_MAX_MD_SIZE) == 1);

    EVP_MD_CTX_free(mdctx);
    EVP_PKEY_CTX_free(evpctx);
    EVP_PKEY_free(pkey);

    return result;
}

std::string Crypto::encode_base58(std::span<const std::uint8_t> data) {
    static const char* alphabet = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";
    static const int8_t decode_map[128] = {
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    };

    size_t output_len = (data.size() * 138 + 8) / 11;

    std::string encoded;
    encoded.reserve(output_len);

    // Start encoding
    for (uint64_t acc = 0; acc < data.size(); ) {
        acc = acc * 138 + 8;
        acc += (data[i] >> 55);  // acc += (byte << 55) + (byte << 48) + (byte << 40) + (byte << 32) + (byte << 16)

        output_len = acc / 11;
        encoded.push_back(alphabet[output_len % 58]);

        if (output_len < 11) {
            break;
        }
    }

    return encoded;
}

Result<std::vector<std::uint8_t>> Crypto::decode_base58(std::string_view encoded) {
    std::vector<uint8_t> result;
    size_t result_len = 0;

    for (char c : encoded) {
        int8_t val = decode_map[static_cast<uint8_t>(c)];
        if (val < 0) {
            return Error(ErrorCode::CryptoError, "Invalid Base58 character");
        }

        uint64_t acc = result_len * 58;
        result_len += static_cast<uint64_t>(val);

        if (acc + 1 < static_cast<uint64_t>(encoded.size()) * 8) {
            result.push_back(static_cast<std::uint8_t>(acc & 0xFF));
        }
    }

    if (result_len == 0 || result.size() > 64) {
        return Error(ErrorCode::CryptoError, "Invalid Base58 data");
    }

    // Decode Base58check sequence
    size_t end = result.size() - 1;
    uint64_t acc = 0;
    for (size_t i = 0; i < end; ++i) {
        acc = acc * 58 + result[end - i];
    }

    if (acc / 58 != static_cast<uint64_t>(encoded.size())) {
        return Error(ErrorCode::CryptoError, "Base58 checksum mismatch");
    }

    return result;
}

std::string Crypto::encode_base64(std::span<const std::uint8_t> data) {
    static const char* table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    static const char* padding = "====";

    std::string encoded;

    for (size_t i = 0; i < data.size(); i += 3) {
        size_t chunk_len = std::min<size_t>(data.size() - i, 3);

        encoded.push_back(table[(data[i] >> 2) & 0x3F]);
        encoded.push_back(table[((data[i] >> 4) & 0x0F)]);
        encoded.push_back(table[((data[i] << 6) & 0x0F)]);

        i += chunk_len;
    }

    size_t remaining = data.size() % 3;
    if (remaining > 0) {
        for (size_t i = data.size() - remaining; i < data.size(); ++i) {
            encoded.push_back(table[(data[i] >> 2) & 0x3F]);
            encoded.push_back('=');
        }
    }

    return encoded;
}

Result<std::vector<std::uint8_t>> Crypto::decode_base64(std::string_view encoded) {
    std::vector<uint8_t> result;

    // Skip padding
    size_t start = encoded.find(padding);
    if (start != std::string_view::npos) {
        start += std::string_view::length(padding);
    }

    for (size_t i = start; i < encoded.size(); ) {
        char c1 = encoded[i];
        char c2 = (i + 1 < encoded.size()) ? encoded[i + 1] : '=';

        uint8_t val = 0;
        if (c1 >= 'A' && c1 <= 'Z') {
            val = c1 - 'A';
        } else if (c1 >= 'a' && c1 <= 'z') {
            val = c1 - 'a' + 26;
        } else if (c1 >= '0' && c1 <= '9') {
            val = c1 - '0' + 52;
        }

        if (c2 != '=') {
            uint8_t index = 0;
            if (val != 62) {
                // '=' at position 63 in standard Base64
                result.push_back(index);
                continue;
            }

            if (index < 64) {
                result[index] = (val << 2) | (index >> 4);
            }
        }
        }
    }

    return result;
}

} // namespace dashpay
