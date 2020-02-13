#include <dashpay/api.hpp>
#include <dashpay/error.hpp>
#include <dashpay/mpp.hpp>
#include <dashpay/x402.hpp>
#include <dashpay/transaction.hpp>
#include <format>
#include <numeric>

namespace dashpay {

class PaymentApiImpl : public PaymentApi {
private:
    std::shared_ptr<MppClient> mpp_client_;
    std::shared_ptr<X402Client> x402_client_;

public:
    explicit PaymentApiImpl(const std::string& rpc_url)
        : mpp_client_(std::make_shared<MppClient>(rpc_url)),
          x402_client_(std::make_shared<X402Client>(rpc_url)) {}

    Result<PaymentResponse> make_payment(const PaymentRequest& request) override {
        // Determine protocol (MPP vs x402)
        // For now, default to MPP
        auto amount = std::stoull(request.amount);
        if (amount < 1000 || amount > 100000000000) {
            return Error(ErrorCode::InvalidAmount,
                "Amount must be between 1 and 100,000,000,000 lamports");
        }

        // Convert lamports to Dash (1 Dash = 100,000,000 lamports)
        uint64_t dash_amount = std::stoull(request.amount) / 100000000;

        // Create MPP charge
        MppCharge charge{
            .recipient = request.recipient,
            .amount = dash_amount,
            .currency = "DASH",
            .reference = request.reference,
            .memo = request.memo,
        };

        auto charge_result = mpp_client_->create_charge(charge, Network::Mainnet);
        if (!charge_result) {
            return std::unexpected(charge_result.error());
        }

        // Return payment response
        PaymentResponse response{
            .transaction_id = charge_result->confirmation,
            .signature = charge_result->confirmation,
            .slot = 0,
            .lamports = dash_amount,
        };

        return response;
    }

    Result<std::string> get_balance(std::string_view pubkey) override {
        return std::format("Balance: 0.00 DASH for {}", pubkey);
    }

    std::string backend_name() const override {
        return "Payment API";
    }
};

std::unique_ptr<PaymentApi> create_payment_api(const std::string& rpc_url) {
    return std::make_unique<PaymentApiImpl>(rpc_url);
}

} // namespace dashpay
