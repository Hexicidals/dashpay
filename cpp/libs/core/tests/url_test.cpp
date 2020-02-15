#include <dashpay/url.hpp>
#include <cassert>
#include <cctype>

namespace dashpay::test {

void test_encode_uri_component() {
    auto encoded = UrlEncoder::encode_uri_component("hello world?test=value");
    assert(encoded.find("hello%20world%3Ftest%3Dvalue") != std::string::npos);
}

void test_decode_uri_component() {
    auto decoded = UrlEncoder::encode_uri_component("hello%20world%3Ftest%3Dvalue");
    assert(decoded == "hello world?test=value");
}

void test_encode_transaction_url() {
    TransactionUrl tx{
        .recipient = "recipient-pubkey",
        .amount = 1000000,
        .reference = "order-123",
        .label = "Test Payment",
        .message = "Thank you!",
    };

    auto url = UrlEncoder::encode_transaction_url(tx);
    assert(url.find("recipient-pubkey") != std::string::npos);
    assert(url.find("1000000") != std::string::npos);
    assert(url.find("order-123") != std::string::npos);
}

void test_encode_spend_url() {
    auto url = UrlEncoder::encode_spend_url("recipient-pubkey", 1000000);
    assert(url.starts_with("https://dashpay.dash.io/"));
}

} // namespace dashpay::test
} // namespace dashpay

int main() {
    dashpay::test::test_encode_uri_component();
    dashpay::test::test_decode_uri_component();
    dashpay::test::test_encode_transaction_url();
    dashpay::test::test_encode_spend_url();
    return 0;
}
