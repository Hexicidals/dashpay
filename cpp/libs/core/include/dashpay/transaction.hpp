#pragma once

#include <cstdint>
#include <optional>
#include <string>

#include "dashpay/error.hpp"
#include "dashpay/url.hpp"

namespace dashpay {

struct Transaction {
    std::string id;
    std::string recipient;
    uint64_t amount;
    std::optional<std::string> reference;
    std::optional<std::string> memo;
    std::string signature;
    std::string blockhash;
    uint64_t slot;
    uint64_t timestamp;

    [[nodiscard]] std::string to_url() const {
        return UrlEncoder::encode_transaction_url({
            .recipient = recipient,
            .amount = amount,
            .reference = reference,
            .label = memo,
            .message = memo,
        });
    }
};

struct TransactionBuilder {
public:
    TransactionBuilder& to(std::string_view recipient) {
        tx_.recipient = recipient;
        return *this;
    }

    TransactionBuilder& amount(uint64_t lamports) {
        tx_.amount = lamports;
        return *this;
    }

    TransactionBuilder& reference(std::string_view ref) {
        tx_.reference = std::string(ref);
        return *this;
    }

    TransactionBuilder& memo(std::string_view memo) {
        tx_.memo = std::string(memo);
        return *this;
    }

    [[nodiscard]] Transaction build() const { return tx_; }

private:
    Transaction tx_;
};

} // namespace dashpay
