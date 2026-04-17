# Wallet Adapter

## Overview

Integration with various wallet adapters for DashPay payments.

## Supported Wallets

### Phantom Wallet

**Setup**:
1. Install Phantom wallet extension
2. Create a DashPay wallet
3. Approve the connection

**Configuration**:
```bash
dashpay setup --wallet phantom
```

### Solflare Wallet

**Setup**:
1. Install Solflare extension
2. Connect to DashPay dApp
3. Sign transactions

### Trust Wallet

**Setup**:
1. Install Trust Wallet
2. Add DashPay network
3. Import or create keypair

### Hardware Wallets

**Ledger**:
1. Connect via USB
2. Approve transactions on device

**Trezor**:
1. Connect via USB
2. Use as source wallet

## Configuration

Wallet adapter configuration is stored in:
`~/.config/dashpay/wallets.toml`

```toml
[phantom]
enabled = true
priority = 1

[solflare]
enabled = true
priority = 2

[trust]
enabled = true
priority = 3
```

## Usage

The CLI automatically detects installed wallets and prompts for connection when needed.

```bash
# Manually trigger wallet detection
dashpay wallets --scan

# Configure default wallet
dashpay wallets --set-default phantom
```
