#include <format>
#include <sstream>

#include "dashpay/url.hpp"
#include "dashpay/encoding.hpp"

namespace dashpay {

std::string UrlEncoder::base_url() {
    return "https://dashpay.dash.io/";
}

std::string UrlEncoder::encode_transaction_url(const TransactionUrl& tx) {
    std::ostringstream url;
    url << base_url();
    url << "?recipient=" << tx.recipient
        << "&amount=" << tx.amount;

    if (tx.reference) {
        url << "&reference=" << Encoding::percent_encode(*tx.reference);
    }
    if (tx.label) {
        url << "&label=" << Encoding::percent_encode(*tx.label);
    }
    if (tx.message) {
        url << "&message=" << Encoding::percent_encode(*tx.message);
    }

    return url.str();
}

std::string UrlEncoder::encode_spend_url(
    std::string_view recipient,
    uint64_t amount,
    std::optional<std::string_view> reference,
    std::optional<std::string_view> label
) {
    std::ostringstream url;
    url << base_url();
    url << "?recipient=" << recipient
        << "&amount=" << amount;

    if (reference && *reference) {
        url << "&reference=" << Encoding::percent_encode(*reference);
    }
    if (label && *label) {
        url << "&label=" << Encoding::percent_encode(*label);
    }

    return url.str();
}

std::string UrlEncoder::encode_uri_component(std::string_view input) {
    std::string result;
    result.reserve(input.size() * 3); // Worst case

    for (char c : input) {
        if (c == ' ') {
            result += "%20";
        } else if (std::isalnum(static_cast<unsigned char>(c)) ||
                     c == '-' || c == '_' || c == '.' || c == '~') {
            result += c;
        } else if (std::isalpha(static_cast<unsigned char>(c))) {
            result += '%';
            result += std::format("{:02X}", static_cast<int>(c));
        } else {
            // Hex encode special characters
            result += std::format("%{:02X}", static_cast<int>(c));
        }
    }

    return result;
}

} // namespace dashpay
