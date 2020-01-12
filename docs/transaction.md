# Transaction Reference Documentation

## Overview

Transactions in DashPay support `reference` field for transaction identification, replacing the legacy `memo` field.

## Reference vs Memo

| Feature | Reference | Memo |
|---------|-----------|-------|
| Primary ID | ✅ Yes | ❌ No |
| Used for lookup | ✅ Yes | ⚠️ Limited |
| Type | string | string |
| Optional | ✅ Yes | ✅ Yes |

## Usage

```cpp
#include <dashpay/transaction.hpp>

using namespace dashpay;

auto tx = TransactionBuilder{}
    .to("7xKXtg2MN87s8y...")
    .amount(1000000)
    .reference("order-12345")
    .build();

std::cout << "Transaction URL: " << tx.to_url() << std::endl;
```

## Transaction URL Format

```
https://dashpay.dash.io/?recipient=<pubkey>&amount=<lamports>&reference=<ref>
```

### Parameters

| Parameter | Description | Required |
|-----------|-------------|----------|
| recipient | Base58 encoded public key | Yes |
| amount | Amount in lamports | Yes |
| reference | Reference string | No |
| label | URL label for display | No |
| memo | Transaction memo (legacy) | No |

## Backward Compatibility

For legacy applications using `memo`, DashPay supports both `reference` and `memo` fields. When both are present, `reference` takes precedence for transaction identification purposes.
