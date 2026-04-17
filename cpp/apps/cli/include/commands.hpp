#pragma once

#include <string>
#include <vector>

#include "dashpay/error.hpp"

namespace cli::commands {

struct CommandResult {
    int exit_code = 0;
    std::string output;
    bool should_print = true;
};

class Command {
public:
    virtual ~Command() = default;

    [[nodiscard]] virtual std::string name() const = 0;
    [[nodiscard]] virtual std::string description() const = 0;

    [[nodiscard]] virtual CommandResult execute(
        const std::vector<std::string>& args
    ) = 0;

    [[nodiscard]] virtual bool requires_keystore() const { return false; }
    [[nodiscard]] virtual bool requires_network() const { return false; }
};

class SetupCommand : public Command {
public:
    [[nodiscard]] std::string name() const override { return "setup"; }
    [[nodiscard]] std::string description() const override {
        return "Initialize a new wallet account";
    }

    [[nodiscard]] CommandResult execute(
        const std::vector<std::string>& args
    ) override;
};

class WhoamiCommand : public Command {
public:
    [[nodiscard]] std::string name() const override { return "whoami"; }
    [[nodiscard]] std::string description() const override {
        return "Display current account information";
    }

    [[nodiscard]] bool requires_keystore() const override { return true; }

    [[nodiscard]] CommandResult execute(
        const std::vector<std::string>& args
    ) override;
};

class SendCommand : public Command {
public:
    [[nodiscard]] std::string name() const override { return "send"; }
    [[nodiscard]] std::string description() const override {
        return "Send a payment";
    }

    [[nodiscard]] bool requires_keystore() const override { return true; }

    [[nodiscard]] CommandResult execute(
        const std::vector<std::string>& args
    ) override;
};

class BalanceCommand : public Command {
public:
    [[nodiscard]] std::string name() const override { return "balance"; }
    [[nodiscard]] std::string description() const override {
        return "Check account balance";
    }

    [[nodiscard]] bool requires_keystore() const override { return true; }

    [[nodiscard]] CommandResult execute(
        const std::vector<std::string>& args
    ) override;
};

class TopupCommand : public Command {
public:
    [[nodiscard]] std::string name() const override { return "topup"; }
    [[nodiscard]] std::string description() const override {
        return "Generate QR code to fund account";
    }

    [[nodiscard]] bool requires_keystore() const override { return false; }

    [[nodiscard]] CommandResult execute(
        const std::vector<std::string>& args
    ) override;
};

} // namespace cli::commands
