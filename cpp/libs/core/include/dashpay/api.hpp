#pragma once

#include <expected>
#include <string>

#include "dashpay/error.hpp"

namespace dashpay {

struct PaymentRequest {
    std::string amount;
    std::string recipient;
    std::optional<std::string> reference;
    std::optional<std::string> memo;
};

struct PaymentResponse {
    std::string transaction_id;
    std::string signature;
    uint64_t slot;
};

class PaymentApi {
public:
    virtual ~PaymentApi() = default;

    [[nodiscard]] virtual Result<PaymentResponse> make_payment(
        const PaymentRequest& request) = 0;

    [[nodiscard]] virtual Result<std::string> get_balance(
        std::string_view pubkey) = 0;
};

} // namespace dashpay
