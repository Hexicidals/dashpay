# Security Policy

## Key Storage

### Keystore Security

All keystore backends implement the following security model:

1. **Authentication Required**: Key access requires user approval
2. **No Clear-Text Storage**: Keys are stored in secure system vaults
3. **Platform Protections**: Uses native OS security APIs
4. **Memory Protection**: Secrets are not exposed in process memory

### File-Based Keystores

File-based keystores (`--keystore file`) are **not secure** and should only be used for:
- Development environments
- Temporary test wallets
- CI/CD pipelines with appropriate permissions

**Never use file keystores for production wallets.**

## Payment Protocols

### MPP (Machine Payments Protocol)

- Charges are created off-chain, settled on-chain
- No direct wallet access for merchants
- Supports reference-based transaction tracking
- Compatible with Solana token extensions

### x402 Payment Protocol

- Challenges are signed, not transmitted as secrets
- Supports identity-based charging
- Compatible with multiple stablecoin providers

## Secrets Management

### Environment Variables

The following environment variables are **never logged**:

| Variable | Purpose |
|-----------|----------|
| `DASHPAY_KEYSTORE_PASSWORD` | Keystore unlock (insecure, avoid) |
| `DASHPAY_CONFIG` | Config file override |

**Never include secrets in:**
- Git commits
- GitHub issues or PRs
- CI/CD logs
- Error messages

### Token Rotation

If you suspect key compromise:

1. Generate new keypair: `dashpay setup`
2. Transfer funds to new key
3. Revoke old key access from keystore

## Reporting Vulnerabilities

If you discover a security vulnerability in DashPay:

1. **Do not create a public issue** - this exposes the vulnerability
2. **Email privately**: security@dashpay.io
3. **Include details**: Steps to reproduce, impact assessment
4. **Responsible disclosure**: Allow reasonable time for fix before disclosure

## Dependencies

DashPay depends on:
- OpenSSL for cryptographic operations
- Platform-native security APIs (Keychain, DPAPI, libsecret, etc.)
- System libraries for base58/QR encoding

All dependencies are audited before inclusion in releases.
