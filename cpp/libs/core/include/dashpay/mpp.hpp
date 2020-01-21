#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "dashpay/error.hpp"
#include "dashpay/types.hpp"

namespace dashpay {

// MPP (Machine Payments Protocol) Implementation
// Based on: https://paymentauth.org/draft-solana-charge-00.html/

struct MppCharge {
    std::string recipient;
    uint64_t amount;
    std::string currency;
    std::optional<std::string> reference;
    std::optional<std::string> memo;
    std::optional<std::string> label;
    std::optional<std::string> message;
    std::string link_key;
    std::optional<std::string> fee_payer;
};

struct MppPayment {
    std::string transaction;
    std::string signature;
    uint64_t slot;
    uint64_t lamports;
    std::string confirmation;
};

class MppClient {
public:
    explicit MppClient(std::string_view rpc_url);

    [[nodiscard]] Result<MppPayment> create_charge(
        const MppCharge& charge,
        Network network
    );

    [[nodiscard]] Result<MppPayment> get_charge(
        std::string_view link_key,
        Network network
    );

    [[nodiscard]] Result<MppPayment> confirm_charge(
        std::string_view transaction,
        Network network
    );

    [[nodiscard]] Result<MppPayment> confirm_payment(
        std::string_view signature,
        Network network
    );

    [[nodiscard]] static std::string encode_link_key(
        std::string_view recipient,
        std::string_view amount
    );

private:
    std::string rpc_url_;
    std::string fee_payer_pubkey_;
};

} // namespace dashpay
