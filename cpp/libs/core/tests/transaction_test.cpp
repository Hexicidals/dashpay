#include <dashpay/transaction.hpp>
#include <cassert>

namespace dashpay {
namespace test {

void test_transaction_builder_chain() {
    auto tx = TransactionBuilder{}
        .to("7xKXtg2MN87s8y...")
        .amount(1000000)
        .reference("test-ref")
        .memo("test-memo")
        .build();

    assert(tx.recipient == "7xKXtg2MN87s8y...");
    assert(tx.amount == 1000000);
    assert(tx.reference == "test-ref");
    assert(tx.memo == "test-memo");
}

void test_transaction_url_generation() {
    auto tx = TransactionBuilder{}
        .to("recipient-pubkey")
        .amount(5000000)
        .reference("order-123")
        .build();

    auto url = tx.to_url();
    assert(url.find("recipient-pubkey") != std::string::npos);
    assert(url.find("5000000") != std::string::npos);
    assert(url.find("order-123") != std::string::npos);
}

} // namespace test
} // namespace dashpay

int main() {
    dashpay::test::test_transaction_builder_chain();
    dashpay::test::test_transaction_url_generation();
    return 0;
}
