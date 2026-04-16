#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include "dashpay/mcp/server.hpp"
#include "dashpay/error.hpp"

namespace dashpay::mcp::test {

TEST(McpServer, basic_lifecycle) {
    McpServerImpl server;
    EXPECT_FALSE(server.is_running());

    server.start();
    EXPECT_TRUE(server.is_running());

    server.stop();
    EXPECT_FALSE(server.is_running());
}

TEST(McpServer, tool_registration) {
    McpServerImpl server;

    McpTool curl_tool{
        .name = "curl",
        .description = "Execute HTTP requests with payment handling",
        .handler = [](const std::string& params) -> std::string {
            return "curl " + params;
        }
    };

    McpTool fetch_tool{
        .name = "fetch",
        .description = "Fetch data from URLs",
        .handler = [](const std::string& params) -> std::string {
            return "fetched " + params;
        }
    };

    server.register_tool(curl_tool);
    server.register_tool(fetch_tool);

    auto tools = server.list_tools();
    EXPECT_EQ(tools.size(), 2ULL);
}

TEST(McpServer, request_handling) {
    McpServerImpl server;
    McpRequest request{
        .method = "tools/list",
        .id = "1",
    };

    auto response = server.handle_request(request);

    EXPECT_FALSE(response.is_error);
    EXPECT_EQ(response.content, R"({"name":"curl","description":"Execute HTTP requests","name":"fetch","description":"Fetch data"})");
}

TEST(McpServer, invalid_request) {
    McpServerImpl server;

    McpRequest request{
        .method = "invalid",
    };

    auto response = server.handle_request(request);

    EXPECT_TRUE(response.is_error);
}

TEST(McpServer, server_start_stop) {
    McpServerImpl server;

    EXPECT_FALSE(server.is_running());

    server.start();
    EXPECT_TRUE(server.is_running());

    auto response = server.handle_request(McpRequest{
        .method = "status",
        .id = "2",
    });

    EXPECT_EQ(response.content, R"({"running":true})");
}

} // namespace dashpay::mcp::test
