#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "dashpay/error.hpp"

namespace cli {

struct CommandContext {
    std::string config_path;
    std::string network;
    bool sandbox = false;
    bool verbose = false;
    bool quiet = false;
    bool auto_pay = false;
};

struct Command {
    std::string name;
    std::vector<std::string> args;
    std::vector<std::string> flags;
    std::string description;
    std::function<int(const CommandContext&)> handler;
};

class CommandLine {
public:
    CommandLine();

    void register_command(Command cmd);
    void register_alias(std::string_view alias, std::string_view command);

    [[nodiscard]] int run(int argc, char* argv[]);

    [[nodiscard]] const Command* find_command(std::string_view name) const;
    [[nodiscard]] std::vector<Command> commands() const;

    void print_help() const;
    void print_version() const;

private:
    std::vector<Command> commands_;
    std::vector<std::pair<std::string, std::string>> aliases_;

    [[nodiscard]] CommandContext parse_arguments(int argc, char* argv[]);
    [[nodiscard]] bool parse_flag(const std::string& arg, std::string_view flag);
};

int run_cli(int argc, char* argv[]);

} // namespace cli
