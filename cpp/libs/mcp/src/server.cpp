#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "dashpay/error.hpp"

namespace dashpay::mcp {

struct McpRequest {
    std::string method;
    std::string params;
    std::string id;
    uint64_t timestamp_ms;
};

struct McpResult {
    std::string content;
    bool is_error = false;
    std::string error_message;
    uint64_t timestamp_ms;
};

struct McpTool {
    std::string name;
    std::string description;
    std::function<Result<std::string>(const std::string&)> handler;
};

class McpServerImpl {
public:
    McpServerImpl();

    void register_tool(const McpTool& tool);
    [[nodiscard]] std::string handle_request(const McpRequest& request);
    [[nodiscard]] std::vector<McpTool> list_tools() const;

    void start();
    void stop();
    [[nodiscard]] bool is_running() const;

private:
    std::unordered_map<std::string, McpTool> tools_;
    bool running_ = false;
    std::mutex tools_mutex_;
};

} // namespace dashpay::mcp
