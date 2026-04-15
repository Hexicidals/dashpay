#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>

#include "dashpay/config.hpp"
#include "dashpay/error.hpp"

namespace dashpay::test {

TEST(ConfigTest, load_default_with_no_file) {
    auto config = Config::load_from_path(std::filesystem::path("/nonexistent/pay.toml"));
    ASSERT_TRUE(config);
    EXPECT_FALSE(config->auto_pay);
    EXPECT_FALSE(config->keypair_path());
    EXPECT_EQ(config->rpc_url_or_default(), LOCAL_RPC_URL);
}

TEST(ConfigTest, load_from_path) {
    // Create temporary config file
    auto temp_path = std::filesystem::temp_directory_path() / "test_config.toml";
    std::ofstream file(temp_path);
    file << "auto_pay = true\n";
    file << "keypair = \"~/.config/dashpay/test.json\"\n";
    file << "rpc_url = \"https://test.rpc.com\"\n";
    file << "log_format = \"json\"\n";
    file.close();

    auto config = Config::load_from_path(temp_path);
    ASSERT_TRUE(config);
    EXPECT_TRUE(config->auto_pay);
    EXPECT_TRUE(config->keypair_path());
    EXPECT_EQ(*config->keypair_path(), "~/.config/dashpay/test.json");
    EXPECT_EQ(config->rpc_url_or_default(), "https://test.rpc.com");

    std::filesystem::remove(temp_path);
}

TEST(ConfigTest, parse_valid_toml) {
    auto temp_path = std::filesystem::temp_directory_path() / "valid_config.toml";
    std::ofstream file(temp_path);
    file << "# Test config\n";
    file << "auto_pay = true\n";
    file << "\n[keystore]\n";
    file << "backend = \"apple-keychain\"\n";
    file << "auth_required = true\n";
    file << "\n[rpc]\n";
    file << "url = \"https://api.dashpay.io\"\n";
    file << "\n[metering]\n";
    file << "enabled = true\n";
    file.close();

    auto config = Config::load_from_path(temp_path);
    ASSERT_TRUE(config);
    EXPECT_TRUE(config->auto_pay);

    std::filesystem::remove(temp_path);
}

TEST(ConfigTest, parse_invalid_toml) {
    auto temp_path = std::filesystem::temp_directory_path() / "invalid_config.toml";
    std::ofstream file(temp_path);
    file << "invalid_syntax = true\n"; // Missing quotes
    file << "keypair = test"; // Missing value
    file.close();

    auto config = Config::load_from_path(temp_path);
    ASSERT_TRUE(config); // Should parse with defaults
    EXPECT_FALSE(config->keypair_path());

    std::filesystem::remove(temp_path);
}

TEST(ConfigTest, rpc_url_fallback_to_default) {
    auto temp_path = std::filesystem::temp_directory_path() / "no_rpc.toml";
    std::ofstream file(temp_path);
    file << "# No RPC URL specified\n";
    file << "auto_pay = false\n";
    file.close();

    auto config = Config::load_from_path(temp_path);
    ASSERT_TRUE(config);
    EXPECT_EQ(config->rpc_url_or_default(), LOCAL_RPC_URL);

    std::filesystem::remove(temp_path);
}

TEST(ConfigTest, keypair_path_ignores_blank_strings) {
    auto config = Config::load_from_path(std::filesystem::path("/nonexistent/pay.toml"));
    ASSERT_TRUE(config);
    EXPECT_FALSE(config->keypair_path()); // No keypair configured
}

TEST(ConfigTest, rpc_url_ignores_blank_strings) {
    auto config = Config::load_from_path(std::filesystem::path("/nonexistent/pay.toml"));
    ASSERT_TRUE(config);
    EXPECT_EQ(config->rpc_url_or_default(), LOCAL_RPC_URL);
}

TEST(ConfigTest, log_format_defaults) {
    auto config = Config::load_from_path(std::filesystem::path("/nonexistent/pay.toml"));
    ASSERT_TRUE(config);
    EXPECT_EQ(config->log_format, LogFormat::Text);
}

TEST(ConfigTest, auto_pay_defaults) {
    auto config = Config::load_from_path(std::filesystem::path("/nonexistent/pay.toml"));
    ASSERT_TRUE(config);
    EXPECT_FALSE(config->auto_pay);
}

} // namespace dashpay::test
