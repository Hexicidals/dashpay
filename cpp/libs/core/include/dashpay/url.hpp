#pragma once

#include <cstdint>
#include <optional>
#include <string>

#include "dashpay/error.hpp"

namespace dashpay {

struct TransactionUrl {
    std::string recipient;
    uint64_t amount;
    std::optional<std::string> reference;
    std::optional<std::string> memo;
    std::optional<std::string> label;
    std::optional<std::string> message;
};

class UrlEncoder {
public:
    [[nodiscard]] static std::string encode_transaction_url(
        const TransactionUrl& tx
    );

    [[nodiscard]] static std::string encode_spend_url(
        std::string_view recipient,
        uint64_t amount,
        std::optional<std::string_view> reference = {},
        std::optional<std::string_view> label = {}
    );

    [[nodiscard]] static std::string encode_uri_component(
        std::string_view input
    );

private:
    [[nodiscard]] static std::string base_url() { return "https://dashpay.dash.io/"; }
};

} // namespace dashpay
