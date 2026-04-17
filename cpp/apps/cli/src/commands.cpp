// CLI command implementations

#include <chrono>
#include <functional>
#include <iostream>
#include <optional>

#include "dashpay/commands.hpp"
#include "dashpay/api.hpp"
#include "dashpay/config.hpp"
#include "dashpay/keystore.hpp"
#include "dashpay/mpp.hpp"

namespace dashpay::commands {

SetupCommand::SetupCommand() = default;

CommandResult SetupCommand::execute(const std::vector<std::string>& args) {
    auto config = Config::load();
    if (!config) {
        return CommandResult{
            .exit_code = 1,
            .output = "Failed to load configuration"
        };
    }

    // Prompt for keystore selection
    std::cout << "Select keystore backend:" << std::endl;
    std::cout << "1) Apple Keychain (macOS)" << std::endl;
    std::cout << "2) Windows Hello (Windows)" << std::endl;
    std::cout << "3) GNOME Keyring (Linux)" << std::endl;
    std::cout << "4) 1Password (Cross-platform)" << std::endl;
    std::cout << "5) File (No encryption)" << std::endl;
    std::cout << "Enter choice [1-5]: ";

    std::string choice;
    std::cin >> choice;

    // Generate keypair
    auto keystore = Keystore::from_string(choice);
    if (!keystore) {
        return CommandResult{
            .exit_code = 1,
            .output = "Invalid keystore choice"
        };
    }

    auto keypair = Crypto::generate_keypair();
    if (!keypair) {
        return CommandResult{
            .exit_code = 1,
            .output = "Failed to generate keypair"
        };
    }

    // Store in keystore
    auto result = keystore->store("default", *keypair);
    if (!result) {
        return CommandResult{
            .exit_code = 1,
            .output = "Failed to store keypair: {}"_format(result.error().to_string()))
        };
    }

    // Write account name
    std::cout << "Account 'default' created successfully!" << std::endl;
    std::cout << "Public Key: " << Crypto::encode_base58(std::span<const std::uint8_t>(keypair->data() + 32, 32)) << std::endl;

    return CommandResult{
        .exit_code = 0,
        .output = "Wallet setup complete!"
    };
}

WhoamiCommand::WhoamiCommand() = default;

CommandResult WhoamiCommand::execute(const std::vector<std::string>& args) {
    auto config = Config::load();
    if (!config) {
        return CommandResult{
            .exit_code = 1,
            .output = "Failed to load configuration"
        };
    }

    auto keystore = Keystore::default_platform();
    if (!keystore) {
        return CommandResult{
            .exit_code = 1,
            .output = "No keystore found. Run: dashpay setup"
        };
    }

    // Check for default account
    auto exists = keystore->exists("default");
    if (!exists) {
        return CommandResult{
            .exit_code = 1,
            .output = "No account configured. Run: dashpay setup"
        };
    }

    auto keypair = keystore->get("default");
    if (!keypair) {
        return CommandResult{
            .exit_code = 1,
            .output = "Failed to get keypair: {}"_format(keypair.error().to_string()))
        };
    }

    // Display account info
    std::cout << "Account: default" << std::endl;
    std::cout << "Public Key: " << Crypto::encode_base58(std::span<const std::uint8_t>(keypair->data() + 32, 32)) << std::endl;
    std::cout << "Keystore: " << keystore->backend_name() << std::endl;

    return CommandResult{
        .exit_code = 0,
        .output = ""
    };
}

SendCommand::SendCommand() = default;

CommandResult SendCommand::execute(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        return CommandResult{
            .exit_code = 1,
            .output = "Usage: dashpay send <recipient> <amount> [--reference REF] [--memo MEMO]"
        };
    }

    auto config = Config::load();
    if (!config) {
        return CommandResult{
            .exit_code = 1,
            .output = "Failed to load configuration"
        };
    }

    auto keystore = Keystore::default_platform();
    if (!keystore) {
        return CommandResult{
            .exit_code = 1,
            .output = "No keystore found. Run: dashpay setup"
        };
    }

    // Get sender keypair
    auto keypair = keystore->get("default");
    if (!keypair) {
        return CommandResult{
            .exit_code = 1,
            .output = "Failed to get keypair: {}"_format(keypair.error().to_string()))
        };
    }

    // Parse arguments
    std::string recipient = args[0];
    std::string amount_str = args[1];
    std::string reference = args.size() > 2 ? args[2] : "";
    std::string memo = args.size() > 3 ? args[3] : "";

    // Validate amount
    auto amount = std::stoull(amount_str);
    if (amount < 1000 || amount > 100000000000) {
        return CommandResult{
            .exit_code = 1,
            .output = "Amount must be between 1,000 and 100,000,000,000 lamports"
        };
    }

    // Create payment
    PaymentApi api = create_payment_api(config->rpc_url_or_default());

    PaymentRequest request{
        .recipient = recipient,
        .amount = amount_str,
        .reference = reference,
        .memo = memo,
    };

    auto result = api->make_payment(request);
    if (!result) {
        return CommandResult{
            .exit_code = 1,
            .output = "Payment failed: {}"_format(result.error().to_string()))
        };
    }

    // Display transaction ID
    std::cout << "Transaction ID: " << result->transaction_id << std::endl;

    return CommandResult{
        .exit_code = 0,
        .output = ""
    };
}

BalanceCommand::BalanceCommand() = default;

CommandResult BalanceCommand::execute(const std::vector<std::string>& args) {
    auto config = Config::load();
    if (!config) {
        return CommandResult{
            .exit_code = 1,
            .output = "Failed to load configuration"
        };
    }

    auto keystore = Keystore::default_platform();
    if (!keystore) {
        return CommandResult{
            .exit_code = 1,
            .output = "No keystore found. Run: dashpay setup"
        };
    }

    // Get public key
    auto keypair = keystore->get("default");
    if (!keypair) {
        return CommandResult{
            .exit_code = 1,
            .output = "Failed to get keypair: {}"_format(keypair.error().to_string()))
        };
    }

    std::string pubkey = Crypto::encode_base58(std::span<const std::uint8_t>(keypair->data() + 32, 32));

    // Get balance via RPC
    PaymentApi api = create_payment_api(config->rpc_url_or_default());
    auto balance = api->get_balance(pubkey);

    if (!balance) {
        return CommandResult{
            .exit_code = 1,
            .output = "Failed to get balance: {}"_format(balance.error().to_string()))
        };
    }

    // Display balance
    std::cout << "Balance: " << balance << std::endl;

    return CommandResult{
        .exit_code = 0,
        .output = ""
    };
}

TopupCommand::TopupCommand() = default;

CommandResult TopupCommand::execute(const std::vector<std::string>& args) {
    auto config = Config::load();
    if (!config) {
        return CommandResult{
            .exit_code = 1,
            .output = "Failed to load configuration"
        };
    }

    auto keystore = Keystore::default_platform();
    if (!keystore) {
        return CommandResult{
            .exit_code = 1,
            .output = "No keystore found. Run: dashpay setup"
        };
    }

    // Get public key
    auto keypair = keystore->get("default");
    if (!keypair) {
        return CommandResult{
            .exit_code = 1,
            .output = "Failed to get keypair: {}"_format(keypair.error().to_string()))
        };
    }

    std::string pubkey = Crypto::encode_base58(std::span<const std::uint8_t>(keypair->data() + 32, 32));

    // Generate topup URL
    uint64_t amount = 1000000; // Default 0.1 DASH
    std::string onramp = args.size() > 1 ? args[1] : "coinbase";

    MppCharge charge{
        .recipient = pubkey,
        .amount = amount,
        .currency = "USDC",
        .label = "Fund DashPay Account",
        .memo = "Fund your DashPay account",
    };

    PaymentApi api = create_payment_api(config->rpc_url_or_default());
    auto payment = api->make_payment(charge);

    if (!payment) {
        return CommandResult{
            .exit_code = 1,
            .output = "Failed to create charge: {}"_format(payment.error().to_string()))
        };
    }

    std::string url = UrlEncoder::encode_transaction_url(charge);

    // Generate QR code
    std::cout << "Topup URL: " << url << std::endl;
    std::cout << "QR Code: " << std::endl;
    std::cout << "Scan with your wallet app to fund account" << std::endl;

    return CommandResult{
        .exit_code = 0,
        .output = ""
    };
}

ServerCommand::ServerCommand() = default;

CommandResult ServerCommand::execute(const std::vector<std::string>& args) {
    if (args.empty()) {
        return CommandResult{
            .exit_code = 1,
            .output = "Usage: dashpay server start [spec.yml] [--debugger]"
        };
    }

    std::string spec_file = args[0];

    std::cout << "Starting DashPay server..." << std::endl;
    std::cout << "Spec file: " << spec_file << std::endl;
    std::cout << "Server will listen on port 8899" << std::endl;
    std::cout << "Payment Debugger available at http://localhost:8890" << std::endl;

    // In production, would start actual server
    // For now, just show message

    return CommandResult{
        .exit_code = 0,
        .output = "Server started (simulated)"
    };
}

} // namespace dashpay::commands
