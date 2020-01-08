#pragma once

#include <cstdint>
#include <optional>
#include <string>

#include "dashpay/error.hpp"

namespace dashpay {

enum class QRErrorCorrection : uint8_t {
    Low,
    Medium,
    Quartile,
    High,
};

struct QRCodeOptions {
    uint8_t version = 0;
    QRErrorCorrection error_correction = QRErrorCorrection::Medium;
    std::optional<uint8_t> margin;
    std::optional<uint8_t> module_size;
    std::optional<uint8_t> border;
};

class QRCode {
public:
    [[nodiscard]] static Result<std::string> encode(
        std::string_view data,
        const QRCodeOptions& options = QRCodeOptions{}
    );

    [[nodiscard]] static Result<std::string> encode_transaction_url(
        std::string_view recipient,
        std::string_view amount,
        std::optional<std::string_view> reference,
        std::optional<std::string_view> label,
        std::optional<std::string_view> message,
        const QRCodeOptions& options = QRCodeOptions{}
    );

private:
    static std::string encode_uri_component(std::string_view input);
};

} // namespace dashpay
