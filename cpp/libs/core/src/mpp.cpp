#include <dashpay/mpp.hpp>
#include <dashpay/rpc.hpp>
#include <dashpay/crypto.hpp>
#include <dashpay/types.hpp>
#include <format>
#include <optional>
#include <random>

namespace dashpay::mpp {

MppClient::MppClient(std::string_view url)
    : rpc_url_(url) {}

Result<MppPayment> MppClient::create_charge(
    const MppCharge& charge,
    Network network
) {
    // Generate a unique link key
    auto link_key = encode_link_key(
        charge.recipient,
        std::format("{}", charge.amount)
    );

    // Construct the RPC call
    auto params = std::format(R"({{
        "method": "mpp_charge",
        "params": [{}",
        "id": "{}"
    }}", link_key, link_key);

    auto response = rpc_client_->send_transaction(params, "");
    if (!response) {
        return std::unexpected(response.error());
    }

    // Parse response to get confirmation
    auto confirmation = std::format(R"({{
        "method": "mpp_confirm_charge",
        "params": [{}]
    }}", link_key);

    MppPayment payment{
        .transaction = response,
        .lamports = charge.amount,
        .confirmation = link_key,
        .currency = charge.currency,
    };

    return payment;
}

Result<MppPayment> MppClient::get_charge(
    std::string_view link_key,
    Network network
) {
    auto params = std::format(R"({{
        "method": "mpp_get_charge",
        "params": [{}]
    }}", link_key);

    auto response = rpc_client_->send_transaction(params, "");
    if (!response) {
        return std::unexpected(response.error());
    }

    auto confirmation = response.data.find("confirmation");
    if (confirmation == std::string::npos) {
        return Error(ErrorCode::MppError, "Invalid MPP charge response");
    }

    MppPayment payment{
        .transaction = "pending",
        .lamports = 0,
        .confirmation = link_key,
        .currency = "DASH",
    };

    return payment;
}

Result<MppPayment> MppClient::confirm_charge(
    std::string_view transaction_id,
    Network network
) {
    auto params = std::format(R"({{
        "method": "mpp_confirm_payment",
        "params": [{}]
    }}", transaction_id);

    auto response = rpc_client_->send_transaction(params, "");
    if (!response) {
        return std::unexpected(response.error());
    }

    // Parse the confirmation
    if (response.code == 0) {
        return Error(ErrorCode::TransactionRejected,
                    "Payment was not confirmed");
    }

    return MppPayment{
        .transaction = transaction_id,
        .signature = "confirmed",
        .slot = static_cast<uint64_t>(std::stoull(response.data)),
        .lamports = 0,
        .confirmation = transaction_id,
        .currency = "DASH",
    };
}

Result<MppPayment> MppClient::confirm_payment(
    std::string_view signature,
    Network network
) {
    // Verify the signature
    // This is a simplified version - in production would verify on-chain

    return MppPayment{
        .transaction = signature,
        .signature = signature,
        .slot = 0,
        .lamports = 0,
        .confirmation = signature,
        .currency = "DASH",
    };
}

std::string MppClient::encode_link_key(
    std::string_view recipient,
    std::string_view amount
) {
    // Base58 encode the link key: mpp:<recipient>:<amount>
    auto base58_recipient = Crypto::encode_base58(
        std::span<const uint8_t>(
            reinterpret_cast<const uint8_t*>(recipient.data()),
            recipient.size()
        )
    );

    auto base58_amount = Crypto::decode_base58(amount);
    if (!base58_amount) {
        return "";
    }

    auto decoded_amount = *base58_amount;
    if (decoded_amount.size() != 8) {
        return "";
    }

    uint64_t amount_value;
    std::memcpy(&amount_value, decoded_amount.data(), 8);

    return std::format("mpp:{}:{}",
        std::string_view(base58_recipient),
        amount_value);
}

} // namespace dashpay::mpp
