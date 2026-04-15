#include <gtest/gtest.h>
#include <dashpay/transaction_validation.hpp>
#include <dashpay/error.hpp>

namespace dashpay::validation::test {

TEST(TransactionAmountValidator, parse_amount_valid_lamports) {
    auto result = TransactionAmountValidator::parse_amount("1000000");
    ASSERT_TRUE(result);
    EXPECT_EQ(*result, 1000000ULL);
}

TEST(TransactionAmountValidator, parse_amount_zero) {
    auto result = TransactionAmountValidator::parse_amount("0");
    ASSERT_TRUE(result);
    EXPECT_EQ(*result, 0ULL);
}

TEST(TransactionAmountValidator, parse_amount_too_small) {
    auto result = TransactionAmountValidator::parse_amount("0.5");
    EXPECT_FALSE(result);
}

TEST(TransactionAmountValidator, parse_amount_too_large) {
    auto result = TransactionAmountValidator::parse_amount("1000000001");
    EXPECT_FALSE(result);
}

TEST(TransactionAmountValidator, parse_amount_negative) {
    auto result = TransactionAmountValidator::parse_amount("-1");
    EXPECT_FALSE(result);
}

TEST(TransactionAmountValidator, parse_amount_invalid_format) {
    auto result = TransactionAmountValidator::parse_amount("abc");
    EXPECT_FALSE(result);
}

TEST(TransactionAmountValidator, parse_dash_amount_whole) {
    auto result = TransactionAmountValidator::parse_dash_amount("1.5");
    ASSERT_TRUE(result);
    EXPECT_DOUBLE_EQ(*result, 1.5);
}

TEST(TransactionAmountValidator, parse_dash_amount_decimal) {
    auto result = TransactionAmountValidator::parse_dash_amount("0.5");
    ASSERT_TRUE(result);
    EXPECT_DOUBLE_EQ(*result, 0.5);
}

TEST(TransactionAmountValidator, parse_dash_amount_zero) {
    auto result = TransactionAmountValidator::parse_dash_amount("0");
    ASSERT_TRUE(result);
    EXPECT_DOUBLE_EQ(*result, 0.0);
}

TEST(TransactionAmountValidator, parse_dash_amount_negative) {
    auto result = TransactionAmountValidator::parse_dash_amount("-1.0");
    EXPECT_FALSE(result);
}

TEST(TransactionAmountValidator, parse_dash_amount_invalid) {
    auto result = TransactionAmountValidator::parse_dash_amount("abc");
    EXPECT_FALSE(result);
}

TEST(TransactionAmountValidator, parse_dash_amount_too_many_decimals) {
    auto result = TransactionAmountValidator::parse_dash_amount("1.23456789");
    EXPECT_FALSE(result);
}

} // namespace dashpay::validation::test
