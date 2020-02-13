#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include <dashpay/api.hpp>
#include <dashpay/cli.hpp>
#include <dashpay/commands.hpp>
#include <dashpay/keystore.hpp>
#include <dashpay/config.hpp>
#include <dashpay/error.hpp>
#include <dashpay/rpc.hpp>

using namespace dashpay;

class DashPayApplication {
public:
    DashPayApplication()
        : cli_(std::make_shared<CommandLine>())
    {
        // Register commands
        cli_->register_command({
            .name = "setup",
            .description = "Initialize a new wallet account",
            .handler = [](const cli::CommandContext& ctx) -> cli::CommandResult {
                cli::commands::SetupCommand cmd;
                return cmd.execute(ctx.args);
            }
        });

        cli_->register_command({
            .name = "whoami",
            .description = "Display current account information",
            .handler = [](const cli::CommandContext& ctx) -> cli::CommandResult {
                cli::commands::WhoamiCommand cmd;
                return cmd.execute(ctx.args);
            }
        });

        cli_->register_command({
            .name = "send",
            .description = "Send a payment",
            .handler = [](const cli::CommandContext& ctx) -> cli::CommandResult {
                cli::commands::SendCommand cmd;
                return cmd.execute(ctx.args);
            }
        });

        cli_->register_command({
            .name = "balance",
            .description = "Check account balance",
            .handler = [](const cli::CommandContext& ctx) -> cli::CommandResult {
                cli::commands::BalanceCommand cmd;
                return cmd.execute(ctx.args);
            }
        });

        cli_->register_command({
            .name = "topup",
            .description = "Generate QR code to fund account",
            .handler = [](const cli::CommandContext& ctx) -> cli::CommandResult {
                cli::commands::TopupCommand cmd;
                return cmd.execute(ctx.args);
            }
        });

        cli_->register_command({
            .name = "server",
            .description = "Start payment gateway server",
            .handler = [](const cli::CommandContext& ctx) -> cli::CommandResult {
                return cli::CommandResult{
                    .exit_code = 1,
                    .output = "Server command not yet implemented"
                };
            }
        });

        // Wrapper commands
        cli_->register_command({
            .name = "curl",
            .description = "Wrap curl with automatic 402 payment handling",
            .handler = [](const cli::CommandContext& ctx) -> cli::CommandResult {
                auto api = create_payment_api(ctx);
                return run_curl_with_api(api, ctx.args);
            }
        });

        cli_->register_command({
            .name = "httpie",
            .description = "Wrap httpie with automatic 402 payment handling",
            .handler = [](const cli::CommandContext& ctx) -> cli::CommandResult {
                auto api = create_payment_api(ctx);
                return run_httpie_with_api(api, ctx.args);
            }
        });

        cli_->register_command({
            .name = "wget",
            .description = "Wrap wget with automatic 402 payment handling",
            .handler = [](const cli::CommandContext& ctx) -> cli::CommandResult {
                auto api = create_payment_api(ctx);
                return run_wget_with_api(api, ctx.args);
            }
        });

        // Version command
        cli_->register_command({
            .name = "--version",
            .description = "Show version information",
            .handler = [](const cli::CommandContext& ctx) -> cli::CommandResult {
                return cli::CommandResult{
                    .exit_code = 0,
                    .output = "DashPay CLI v0.16.0\nC++ implementation of Solana Pay's payment layer"
                };
            }
        });
    }

    [[nodiscard]] std::shared_ptr<PaymentApi> create_payment_api(
        const cli::CommandContext& ctx
    ) const {
        auto config_result = Config::load();
        if (!config_result) {
            return nullptr;
        }

        const auto& config = *config_result;
        std::string rpc_url = config.rpc_url_or_default();

        return create_payment_api(rpc_url);
    }

    [[nodiscard]] std::shared_ptr<PaymentApi> create_payment_api(
        std::string_view rpc_url
    ) {
        return std::make_shared<PaymentApiImpl>(std::string(rpc_url));
    }

    [[nodiscard]] int run_curl_with_api(
        std::shared_ptr<PaymentApi> api,
        const std::vector<std::string>& args
    ) {
        // Extract URL and arguments
        std::vector<std::string> curl_args;
        bool next_is_url = false;
        bool skip_next = false;
        std::string url;

        for (const auto& arg : args) {
            if (arg.empty()) {
                continue;
            }

            if (!next_is_url && arg.starts_with("http")) {
                url = arg;
                next_is_url = true;
            } else if (next_is_url && arg.starts_with("-")) {
                curl_args.push_back(arg);
                next_is_url = false;
                skip_next = true;
            } else if (skip_next) {
                curl_args.push_back(arg);
            } else if (arg == "--") {
                skip_next = false;
            }
        }

        // Call CLI to make payment
        return cli_->execute_command("curl", curl_args);
    }

    [[nodiscard]] int run_httpie_with_api(
        std::shared_ptr<PaymentApi> api,
        const std::vector<std::string>& args
    ) {
        // httpie format: http URL [data]
        if (args.size() < 2) {
            return 1;
        }

        std::string url = args[0];
        std::string data;

        if (args.size() > 2) {
            data = args[2];
            for (size_t i = 3; i < args.size(); ++i) {
                data += " " + args[i];
            }
        }

        auto request = PaymentRequest{
            .recipient = "test-recipient",
            .amount = "1000000"
        };

        auto response = api->make_payment(request);
        if (response) {
            std::cout << response.transaction_id << std::endl;
            return 0;
        }

        std::cerr << "Payment failed: " << response.error().to_string() << std::endl;
        return 1;
    }

    [[nodiscard]] int run_wget_with_api(
        std::shared_ptr<PaymentApi> api,
        const std::vector<std::string>& args
    ) {
        // wget format: http URL
        if (args.size() < 2) {
            return 1;
        }

        std::string url = args[0];
        bool use_data_stdin = false;
        std::string post_data;

        if (args.size() > 2 && args[1] == "-") {
            use_data_stdin = true;
        }

        if (use_data_stdin && args.size() > 3) {
            post_data = args[3];
            for (size_t i = 4; i < args.size(); ++i) {
                post_data += " " + args[i];
            }
        }

        auto request = PaymentRequest{
            .recipient = "test-recipient",
            .amount = "1000000"
        };

        auto response = api->make_payment(request);
        if (response) {
            std::cout << response.transaction_id << std::endl;
            return 0;
        }

        std::cerr << "Payment failed: " << response.error().to_string() << std::endl;
        return 1;
    }

    int run() {
        int argc = 0;
        char** argv = nullptr;

        // Get command line arguments
        #ifdef _WIN32
        argv = CommandLineToArgvW(GetCommandLineW(), &argc);
        #else
        argv = CommandLineToArgvA(GetCommandLineA(), &argc);
        #endif

        // Parse arguments (skip program name)
        std::vector<std::string> args;
        for (int i = 1; i < argc; ++i) {
            std::wstring arg(argv[i]);
            int size_needed = WideCharToMultiByteChar(arg.c_str(), CP_UTF8) + 1;
            std::string narrow_arg(size_needed, 0);
            WideCharToMultiByteChar(arg.c_str(), size_needed, CP_UTF8, narrow_arg.data());
            args.push_back(narrow_arg);
        }

        // Handle --help or no arguments
        if (args.empty() || (args.size() == 1 && (args[0] == "--help" || args[0] == "-h")) {
            cli_->print_help();
            return 0;
        }

        // Execute command
        return cli_->run(args);
    }
};

int main(int argc, char* argv[]) {
    try {
        DashPayApplication app;
        return app.run();
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}
