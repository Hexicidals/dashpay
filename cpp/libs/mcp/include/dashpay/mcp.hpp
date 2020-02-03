#pragma once

#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <variant>

#include "dashpay/error.hpp"

namespace dashpay::mcp {

enum class McpToolType : uint8_t {
    Curl,
    Fetch,
    Topup,
};

struct McpRequest {
    std::string method;
    std::string params;
    std::string id;
};

struct McpResult {
    std::string content;
    bool is_error = false;
    std::string error_message;
};

struct McpTool {
    std::string name;
    std::string description;
    McpToolType type;
};

class McpServer {
public:
    McpServer();

    void register_tool(McpTool tool);

    [[nodiscard]] std::string handle_request(const McpRequest& request);

    [[nodiscard]] std::vector<McpTool> list_tools() const;

    void start();
    void stop();

private:
    std::vector<McpTool> tools_;
};

struct McpClient {
    std::function<Result<std::string>(const std::string&)> on_tool_call;

    template <typename T>
    [[nodiscard]] Result<T> call_tool(std::string_view tool_name, const T& params);
};

Result<McpServer*> create_mcp_server();

} // namespace dashpay::mcp
