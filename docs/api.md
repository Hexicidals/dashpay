# Payment API Documentation

## Payment Request

```cpp
struct PaymentRequest {
    std::string amount;
    std::string recipient;
    std::optional<std::string> reference;
    std::optional<std::string> memo;
};
```

### Fields

| Field | Type | Description |
|-------|--------|-------------|
| amount | string | Payment amount in base units |
| recipient | string | Base58 encoded public key |
| reference | optional&lt;string&gt; | Optional reference account for transaction identification |
| memo | optional&lt;string&gt; | Optional memo field |

## Payment Response

```cpp
struct PaymentResponse {
    std::string transaction_id;
    std::string signature;
    uint64_t slot;
};
```

### Fields

| Field | Type | Description |
|-------|--------|-------------|
| transaction_id | string | Transaction signature |
| signature | string | Transaction signature |
| slot | uint64_t | Confirmation slot |

## Usage

```cpp
#include <dashpay/payment.hpp>

using namespace dashpay;

PaymentApi* api = create_payment_api();

PaymentRequest request{
    .amount = "1000000",
    .recipient = "7xKXtg2MN87s8y...",
    .reference = "order_12345"
};

auto response = api->make_payment(request);
if (response) {
    std::cout << "Payment successful: " << response->transaction_id << std::endl;
} else {
    std::cerr << "Payment failed: " << response.error() << std::endl;
}
```
