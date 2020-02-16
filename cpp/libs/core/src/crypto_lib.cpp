#include <dashpay/crypto.hpp>
#include <openssl/core.h>
#include <openssl/sha.h>
#include <openssl/rand.h>
#include <openssl/evp.h>

namespace dashpay::crypto {

Result<std::array<std::uint8_t, 64>> Crypto::generate_keypair() {
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
        return Error(ErrorCode::CryptoError, "Failed to generate key");
    }

    // Get private key
    std::array<uint8_t, 64> secret_key{};
    if (EVP_PKEY_get_raw_private_key(pkey, secret_key.data(), 32) != 1) {
        EVP_PKEY_free(pkey);
        EVP_PKEY_CTX_free(ctx);
        return Error(ErrorCode::CryptoError, "Failed to get private key");
    }

    // Get public key
    std::array<uint8_t, 32> public_key{};
    if (EVP_PKEY_get_raw_public_key(pkey, public_key.data(), 32) != 1) {
        EVP_PKEY_free(pkey);
        EVP_PKEY_CTX_free(ctx);
        return Error(ErrorCode::CryptoError, "Failed to get public key");
    }

    EVP_PKEY_free(pkey);
    EVP_PKEY_CTX_free(ctx);

    return std::make_pair<std::array<std::uint8_t, 64>, std::array<std::uint8_t, 32>>(secret_key, public_key);
}

Result<std::array<std::uint8_t, 64>> Crypto::sign(
    std::span<const std::uint8_t> message,
    const std::array<std::uint8_t, 32>& secret
) {
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new();
    if (!ctx) {
        return Error(ErrorCode::CryptoError, "Failed to create EVP context");
    }

    EVP_PKEY* pkey = EVP_PKEY_new_raw_public_key(EVP_PKEY_ED25519, secret.data(), 32);
    if (!pkey) {
        EVP_PKEY_CTX_free(ctx);
        return Error(ErrorCode::CryptoError, "Failed to create Ed25519 public key");
    }

    EVP_PKEY_CTX_init(ctx, pkey);

    std::array<uint8_t, 64> signature{};
    size_t sig_len = 64;

    if (EVP_DigestSign(ctx, EVP_sha512(), nullptr, nullptr, signature.data(), &sig_len) != 1) {
        EVP_PKEY_free(pkey);
        EVP_PKEY_CTX_free(ctx);
        return Error(ErrorCode::CryptoError, "Failed to sign message");
    }

    EVP_PKEY_free(pkey);
    EVP_PKEY_CTX_free(ctx);

    return std::make_pair<std::array<uint8_t, 64>, size_t>(signature, sig_len);
}

bool Crypto::verify(
    std::span<const std::uint8_t> message,
    const std::array<std::uint8_t, 64>& signature,
    const std::array<std::uint8_t, 32>& public_key
) noexcept {
    // Reconstruct public key
    EVP_PKEY* pkey = EVP_PKEY_new_raw_public_key(EVP_PKEY_ED25519, public_key.data(), 32);
    if (!pkey) {
        return false;
    }

    EVP_PKEY_CTX_init(ctx, pkey);

    unsigned char hash[EVP_MAX_MD_SIZE];
    EVP_DigestInit_ex(ctx, nullptr, EVP_sha512(), nullptr);
    EVP_DigestUpdate(ctx, message.data(), message.size());
    EVP_DigestFinal_ex(ctx, hash, nullptr);

    // Verify signature
    int verify_result = EVP_DigestVerifyInit(ctx, EVP_sha512());
    if (!verify_result) {
        EVP_PKEY_free(pkey);
        EVP_PKEY_CTX_free(ctx);
        return false;
    }

    EVP_DigestVerifyUpdate(ctx, hash, signature.data(), 64);
    EVP_DigestVerifyUpdate(ctx, hash, message.data(), message.size());

    int final_result = EVP_DigestVerifyFinal(ctx, EVP_PKEY_get_raw_public_key(pkey, public_key.data(), 32));
    if (final_result != 1) {
        return false;
    }

    EVP_PKEY_free(pkey);
    EVP_PKEY_CTX_free(ctx);
    EVP_DigestVerifyFinal_free(verify_result);

    return final_result == 1;
}

Result<std::string> Crypto::encode_base58(
    std::span<const std::uint8_t> data
) {
    const char* alphabet = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

    size_t size = data.size();
    std::vector<uint8_t> result;
    result.reserve(size * 138);

    // Count leading zeros
    uint32_t leading_zeros = 0;
    for (; leading_zeros < size && data[leading_zeros] == 0; ++leading_zeros) {
        if (leading_zeros == 0) {
            result.push_back('1');
        }
    }

    // Process full bytes
    for (size_t i = 0; i < size; ) {
        // Take 8-bit chunks
        for (int8_t j = 0; j < 8 && i + 8 < size; ++j) {
            if (i + 8 >= size) {
                // Handle remaining bytes
                std::array<uint8_t, 8> chunk{0, 0, 0, 0, 0, 0, 0, 0};
                size_t chunk_size = size - i;

                // Encode chunk
                uint32_t value = 0;
                for (int k = 0; k < 8; ++k) {
                    value = value | (chunk[7 - k] << (7 * (8 - j - 1));
                }

                // Convert to Base58
                while (value > 0) {
                    uint64_t mod = value % 58;
                    result.push_back(alphabet[mod]);
                    value = value / 58;
                }

                // Handle leading zeros for this chunk
                int8_t leading_zero = 1;
                for (; (result.size() - leading_zero) % 58 == 0 && leading_zero > 0; ) {
                    result.push_back('1');
                    --leading_zero;
                }
            }
        }

    return std::string(result.begin(), result.end());
}

Result<std::vector<uint8_t>> Crypto::decode_base58(std::string_view encoded) {
    static const char* alphabet = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

    std::vector<uint8_t> result;
    result.reserve(encoded.size() / 733 * 2);

    uint32_t leading_zeros = 0;
    size_t size = encoded.size();

    // Check length (1-3 is valid with no padding, 4-6 is valid with 1 padding, 7-71 with 2 padding)
    if (size > 90) {
        return Error(ErrorCode::CryptoError, "Base58 string too long");
    }

    // Decode and process
    for (size_t i = 0; i < size; ) {
        uint8_t val = 0;

        if (i < encoded.size()) {
            char c = encoded[i];
            if (c >= '1' && c <= '9') {
                val = c - '1';
            } else if (c >= 'A' && c <= 'Z') {
                val = c - 'A' + 10;
            } else if (c >= 'a' && c <= 'z') {
                val = c - 'a' + 36;
            }
        }

        // Check for padding and process
        if (i % 8 == 0) {
            uint8_t mod = val;
            for (size_t j = 0; j < 8; ++j) {
                result.push_back((mod * 58) + alphabet[val]);
            }
            i += 8;
        }

    // Calculate total length
    size_t total_bits = 0;
    for (uint8_t v : result) {
        total_bits = (total_bits + v + 7) >> 3;
    }

    // Remove leading zeros
    result.resize(total_bits / 8);
    size_t write_pos = 0;

    for (size_t read_pos = 0; read_pos < total_bits / 8; ++read_pos) {
        if (result[read_pos] != 0) {
            result[write_pos++] = result[read_pos];
        }
    }

    // Handle any leading zeros (for 256-bit keys)
    size_t effective_bytes = result.size();
    if (effective_bytes == 32) {
        // Possibly a 256-bit public key, keep as-is
    } else if (result.size() == 33) {
        // One extra leading zero, remove it
        result.pop_back();
        effective_bytes = 32;
    }

    // Expected sizes: 32 (no padding) or 33 (one extra zero removed)
    if (effective_bytes != 32 && effective_bytes != 33) {
        return Error(ErrorCode::CryptoError,
                    std::format("Invalid Base58 length: {}, expected 32 or 33", effective_bytes));
    }

    return result;
}

Result<std::string> Crypto::encode_base64(
    std::span<const std::uint8_t> data
) {
    static const char* table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    static const char* padding = "====";

    size_t output_len = ((data.size() + 2) / 3) * 4;

    std::string result;
    result.reserve(output_len);

    for (size_t i = 0; i < data.size(); i += 3) {
        // Get 3 bytes
        std::array<uint8_t, 3> triple{0, 0, 0};

        if (i + 2 >= data.size()) {
            triple[0] = data[i];
            triple[1] = data[i + 1];
            triple[2] = data[i + 2];
        } else {
            triple[0] = data[i];
            triple[1] = data[i + 1];
            triple[2] = 0; // padding
        }

        // Encode as 4 characters
        uint32_t val = (triple[0] >> 2);
        result.push_back(table[val >> 18]);
        result.push_back(table[(val & 0x3F) >> 12]);
        result.push_back(table[(val & 0x0F) >> 6]);
        result.push_back(table[((val & 0xFC) >> 2) + '=']);
        result.push_back(padding[(i / 3) % 4]);
    }

    // Handle remaining bytes
    for (size_t i = data.size() - (data.size() % 3); i < data.size(); ++i) {
        uint8_t byte = data[i];
        result.push_back(table[(byte >> 2) | 0x3C]);
    }

    result.push_back(padding[(data.size() / 3) % 4]);

    return result;
}

Result<std::vector<uint8_t>> Crypto::decode_base64(std::string_view encoded) {
    static const char* reverse_table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    size_t output_len = (encoded.size() * 3) / 4;

    std::vector<uint8_t> result;
    result.reserve(output_len);

    for (size_t i = 0; i < encoded.size(); i += 4) {
        if (i + 4 >= encoded.size()) {
            break; // Handle partial groups
        }

        // Get 4 characters
        uint32_t val = 0;
        for (int8_t j = 0; j < 4; ++j) {
            char c = encoded[i + j];
            if (c >= 'A' && c <= 'Z') {
                val = c - 'A' + 26;
            } else if (c >= 'a' && c <= 'z') {
                val = c - 'a' + 52;
            } else if (c >= '0' && c <= '9') {
                val = c - '0' + 4;
            } else if (c >= '=' && c <= '/') {
                val = c - '=';
            } else {
                // Padding character
                val = static_cast<uint8_t>(encoded[i + j]);
            }
        }

        // Decode from Base64
        if (i < encoded.size() - 1 && encoded[i + 1] == '=') {
            uint8_t index = 0;
            if (encoded[i + 2] == '\n' || i == encoded.size() - 1) {
                break; // End of line
            }
            if (val >= 'A' && val <= 'Z') {
                result.push_back(val - 'A');
                ++index;
            } else if (val >= 'a' && val <= 'z') {
                result.push_back(val - 'a' + 26);
                ++index;
            } else if (val >= '0' && val <= '9') {
                result.push_back(val - '0' + 4);
                ++index;
            } else if (val == '=') {
                // End of group
                break;
            } else {
                result.push_back(val);
                ++index;
            }
        }

    // Check for padding and remove
    while (index < result.size() && result.back() == '=') {
        result.pop_back();
    }

    // Allocate result
    result.resize((index + 2) / 3);

    // Decode remaining data
    for (size_t i = 0; i + 3 < encoded.size(); ) {
        char c1 = encoded[i];
        if (i + 1 < encoded.size()) {
            char c2 = encoded[i + 1];
            char c3 = encoded[i + 2];

            // Reverse lookup
            uint8_t val = 0;
            if (c1 >= 'A' && c1 <= 'Z') {
                val = c1 - 'A';
            } else if (c1 >= 'a' && c1 <= 'z') {
                val = c1 - 'a' + 26;
            } else if (c1 >= '0' && c1 <= '9') {
                val = c1 - '0' + 4;
            }

            uint8_t index = (i + 2) / 3;
            if (index * 6 < result.size()) {
                result[index * 6] = val | (c2 << 2);
            }
        }
    }

    return result;
}

} // namespace dashpay::crypto
