#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "dashpay/error.hpp"

namespace dashpay::qrcode {

enum class QRLevel : uint8_t {
    L = 1,    // ~7% correction
    M = 2,    // ~15% correction
    Q = 3,    // ~25% correction
    H = 4,    // ~30% correction
};

struct QRGenerationOptions {
    std::optional<QRLevel> level = QRLevel::M;
    uint8_t margin = 2;
    uint8_t cell_size = 5;
    bool quiet_zone = false;
    bool with_logo = true;
};

class QRCode {
public:
    static std::string generate_transaction_qr(
        const std::string& recipient,
        uint64_t lamports,
        const std::optional<std::string>& reference
    );

    static std::string generate_amount_qr(uint64_t lamports);
    static std::string generate_topup_qr(
        uint64_t lamports,
        const std::optional<std::string>& method,
        const std::optional<std::string>& amount
    );
};

class QREncoder {
public:
    static std::string encode_transaction(const QRCode& code);
    static std::string encode_amount(uint64_t lamports);
    static std::string encode_topup(uint64_t lamports);

    [[nodiscard]] static std::string to_url(const QRCode& code);
};

} // namespace dashpay::qrcode
