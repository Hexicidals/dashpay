# MPP (Machine Payments Protocol)

## Overview

MPP enables programmatic payments with stablecoins on Solana. Charges are created as off-chain metadata that can be settled on-chain at any point.

## Creating a Charge

```cpp
#include <dashpay/mpp.hpp>

using namespace dashpay;

MppClient client("https://api.dash.org");

MppCharge charge{
    .recipient = "7xKXtg2MN87s8y...",
    .amount = 10000000,        // 0.1 USDC
    .currency = "USDC",
    .reference = "order-12345",
    .label = "Coffee Shop",
    .message = "Thank you!",
};

auto payment = client.create_charge(charge, Network::Mainnet);
if (payment) {
    std::cout << "Link key: " << payment->confirmation << std::endl;
}
```

## Charge Flow

1. **Create charge**: Call `create_charge()` with payment details
2. **Get link key**: Returns a link key that references the charge
3. **User approves**: User signs transaction in their wallet
4. **Confirm charge**: Call `confirm_charge()` to finalize on-chain

## Fields

| Field | Type | Description |
|-------|--------|-------------|
| recipient | string | Base58 encoded public key |
| amount | uint64_t | Amount in minor units (e.g., USDC cents) |
| currency | string | Stablecoin ticker (USDC, PYUSD, etc.) |
| reference | optional&lt;string&gt; | Order reference or payment ID |
| memo | optional&lt;string&gt; | Transaction memo |
| label | optional&lt;string&gt; | Display label for wallet UI |
| message | optional&lt;string&gt; | Custom message for approval |
| link_key | string | Unique identifier for charge lookup |
| fee_payer | optional&lt;string&gt; | Public key of fee payer |

## Link Key Format

```
https://pay.solana.com/<link_key>
```
