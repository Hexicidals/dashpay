#include <dashpay/x402.hpp>
#include <dashpay/rpc.hpp>
#include <dashpay/crypto.hpp>
#include <dashpay/types.hpp>
#include <format>
#include <optional>
#include <regex>

namespace dashpay::x402 {

X402Client::X402Client(const std::string& url)
    : rpc_url_(url) {}

Result<X402Challenge> X402Client::parse_challenge(
    const std::vector<std::pair<std::string, std::string>>& headers
) {
    // Find x402-payment header
    std::string payment_header;
    std::string challenge_base64;

    for (const auto& [name, value] : headers) {
        if (name == "x402-payment" || name == "x402-challenge") {
            payment_header = value;
            break;
        }
    }

    if (payment_header.empty()) {
        return Error(ErrorCode::X402Error,
                    "No x402 payment header found in response");
    }

    // Parse the header format
    // x402-payment: <protocol>:<base64>
    std::regex header_re(R"(x402-[^:]+):(.+)");
    std::smatch match = std::regex_match(payment_header, header_re);

    if (!match || !match->ready()) {
        return Error(ErrorCode::X402Error,
                    "Failed to parse x402 header: {}"_format(payment_header));
    }

    auto protocol = match->str(1);
    auto challenge_b64 = match->str(2);

    // Parse challenge parameters
    auto params = parse_x402_challenge(challenge_b64, protocol);

    X402Challenge challenge{
        .header = payment_header,
        .challenge = challenge_b64,
        .protocol = std::string(protocol),
        .network = params.network,
        .currency = params.currency,
        .amount = params.amount,
        .recipient = params.recipient,
        .reference = params.reference,
    };

    return challenge;
}

Result<X402Challenge> X402Client::parse_x402_challenge(
    std::string_view challenge_base64,
    std::string_view protocol
) {
    // Decode base64 challenge
    auto decoded = Encoding::base64_decode(challenge_base64);
    if (!decoded) {
        return Error(ErrorCode::X402Error,
                    "Failed to decode x402 challenge");
    }

    // Parse as JSON object
    // Expected format: {
    //   "challenge": "<base64>",
    //   "network": "<network>",
    //   "currency": "<currency>",
    //   "amount": <amount>,
    //   "recipient": "<recipient>",
    //   "reference": "<optional reference>",
    //   "memo": "<optional memo>"
    // }

    std::string challenge_str(decoded->begin(), decoded->end());

    // Simple JSON parsing for challenge
    auto network_pos = challenge_str.find("\"network\":");
    if (network_pos == std::string::npos) {
        return Error(ErrorCode::X402Error, "Network not found in challenge");
    }

    auto network_start = challenge_str.find('"', network_pos + 11);
    auto network_end = challenge_str.find('"', network_start);
    if (network_end == std::string::npos) {
        return Error(ErrorCode::X402Error, "Network not found in challenge");
    }

    std::string network(challenge_str.substr(network_start + 2, network_end - network_start));

    // Parse other fields
    std::string recipient, currency;
    uint64_t amount = 0;
    std::optional<std::string> reference;
    std::optional<std::string> memo;

    // Parse recipient
    auto recipient_pos = challenge_str.find("\"recipient\":");
    if (recipient_pos != std::string::npos) {
        auto recipient_start = recipient_pos + 14;
        auto recipient_end = challenge_str.find('"', recipient_start);
        recipient = challenge_str.substr(recipient_start + 2, recipient_end - recipient_start - 2);
    }

    // Parse currency
    auto currency_pos = challenge_str.find("\"currency\":");
    if (currency_pos != std::string::npos) {
        auto currency_start = currency_pos + 12;
        auto currency_end = challenge_str.find('"', currency_start);
        currency = challenge_str.substr(currency_start + 2, currency_end - currency_start - 2);
    }

    // Parse amount
    auto amount_pos = challenge_str.find("\"amount\":");
    if (amount_pos != std::string::npos) {
        auto amount_start = amount_pos + 10;
        auto amount_end = challenge_str.find(',', amount_start);
        if (amount_end != std::string::npos) {
            try {
                amount = static_cast<uint64_t>(std::stoull(challenge_str.substr(amount_start + 2, amount_end - amount_start - 2)));
            } catch (...) {
                return Error(ErrorCode::InvalidAmount, "Invalid amount format");
            }
        }
    }

    // Parse reference (optional)
    auto reference_pos = challenge_str.find("\"reference\":");
    if (reference_pos != std::string::npos) {
        auto reference_start = reference_pos + 14;
        auto reference_end = challenge_str.find('"', reference_start);
        reference = challenge_str.substr(reference_start + 2, reference_end - reference_start - 2);
    }

    // Parse memo (optional)
    auto memo_pos = challenge_str.find("\"memo\":");
    if (memo_pos != std::string::npos) {
        auto memo_start = memo_pos + 9;
        auto memo_end = challenge_str.find('"', memo_start);
        memo = challenge_str.substr(memo_start + 2, memo_end - memo_start - 2);
    }

    X402Params params{
        .network = network,
        .currency = currency,
        .amount = amount,
        .recipient = recipient,
        .reference = reference,
        .memo = memo,
    };

    return params;
}

Result<X402Payment> X402Client::sign_challenge(
    const X402Challenge& challenge,
    const SecretKey& secret_key
) {
    // Decode the challenge
    auto challenge_bytes = Encoding::base64_decode(challenge.challenge);
    if (!challenge_bytes || challenge_bytes->size() < 64) {
        return Error(ErrorCode::X402Error, "Invalid challenge format");
    }

    // Sign with Ed25519
    auto signature = Crypto::sign(
        std::span<const uint8_t>(challenge_bytes->data(), challenge_bytes->size()),
        secret_key
    );

    if (!signature) {
        return std::unexpected(signature.error());
    }

    X402Payment payment{
        .signature = Crypto::encode_base58(
            std::span<const uint8_t>(signature->data(), 32)
        ),
        .proof = "x402-proof: " + Crypto::encode_base58(
            std::span<const uint8_t>(signature->data(), 64)
        ),
    };

    return payment;
}

Result<X402Payment> X402Client::submit_payment(
    const X402Challenge& challenge,
    const X402Payment& payment
) {
    // In a real implementation, this would:
    // 1. Send the proof to the server
    // 2. Get the response with the payment proof
    // For now, return a simulated response

    X402Payment response{
        .transaction_id = "simulated-" + payment.signature,
        .signature = payment.signature,
        .proof = payment.proof,
    };

    return response;
}

Result<std::string> X402Client::is_x402_response(
    int status_code,
    const std::vector<std::pair<std::string, std::string>>& headers
) {
    // Check for x402 payment protocol headers
    for (const auto& [name, value] : headers) {
        if (name == "x402-payment" || name == "x402-challenge" ||
            name == "x402-proof" || name == "x402-status") {
            return true;
        }
    }

    return false;
}

std::unique_ptr<X402Client> X402Client::default_platform() {
#if defined(__APPLE__)
    return std::make_unique<X402Client>("https://api.dash.org");
#elif defined(_WIN32)
    return std::make_unique<X402Client>("https://api.dash.org");
#elif defined(__linux__)
    return std::make_unique<X402Client>("https://api.dash.org");
#else
    return nullptr;
}

} // namespace dashpay::x402
