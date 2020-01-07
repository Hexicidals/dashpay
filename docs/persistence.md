# Persistence Layer Documentation

## Overview

The persistence layer provides transaction history tracking and storage for the DashPay CLI.

## TransactionRecord

```cpp
struct TransactionRecord {
    std::string id;
    std::string from;
    std::string to;
    uint64_t amount;
    uint64_t timestamp;
    std::string signature;
};
```

## Persistence Interface

```cpp
class Persistence {
public:
    virtual ~Persistence() = default;

    [[nodiscard]] virtual Result<std::vector<TransactionRecord>> get_history(
        std::string_view pubkey) = 0;

    virtual Result<void> save_transaction(
        const TransactionRecord& record) = 0;

    [[nodiscard]] virtual Result<TransactionRecord*> get_transaction(
        std::string_view id) = 0;
};
```

## FilePersistence

Default implementation using JSON file storage.

### Data Format

```json
{
    "transactions": [
        {
            "id": "transaction_signature",
            "from": "sender_pubkey",
            "to": "recipient_pubkey",
            "amount": 1000000,
            "timestamp": 1609459200,
            "signature": "base58_signature"
        }
    ]
}
```

### Storage Location

- **Linux/macOS**: `~/.config/dashpay/transactions.json`
- **Windows**: `%APPDATA%/dashpay/transactions.json`
