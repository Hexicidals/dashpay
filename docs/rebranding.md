# Rebranding from Solana Pay to DashPay

This document describes the rebranding changes made from the original Solana Pay codebase to DashPay.

## Changes Made

### Branding

| Old Name | New Name |
|-----------|----------|
| pay | dashpay |
| pay.sh | dashpay |
| @solana/pay | @dashpay/cli |
| solana-foundation | dashpay-org |
| pay-skills | dashpay-skills |
| solanapay.com | dashpay.io |
| debugger.pay.sh | debugger.dashpay.io |

### Network References

| Old URL | New URL |
|----------|----------|
| https://pay.solana.com | https://pay.dashpay.io |
| https://api.solana.com | https://api.dashpay.io |
| https://402.surfnet.dev | https://402.dashpay.io |

### Repository Structure

All directory and file references have been updated to use dashpay naming conventions.

### Code Comments

All references to "Solana", "pay", "Solana Pay" have been updated to "Dash", "dashpay", "DashPay" respectively.

### Documentation

All documentation has been updated to reflect DashPay branding instead of Solana Pay.

### Configuration

- Config files now use `DASHPAY_CONFIG` environment variable
- Accounts directory: `~/.config/dashpay/` (was `~/.config/pay/`)
- Default keystore service: `org.dashpay` (was `com.solana`)

## Compatibility

The rebranding maintains full protocol compatibility:
- MPP protocol unchanged
- x402 protocol unchanged
- Same cryptographic primitives (ed25519, base58)
- Same RPC endpoints

## Migration

For users upgrading from Solana Pay:

1. Install DashPay: `npm install -g @dashpay/cli`
2. Config files are in new location: `~/.config/dashpay/`
3. Run `dashpay setup` to reconfigure your wallet

Legacy Solana Pay keys are not automatically migrated - you'll need to re-setup your account.
