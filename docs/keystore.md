# Keystore Documentation

## Overview

DashPay provides secure key storage with multiple backend options:

| Backend | Platform | Auth Method |
|---------|----------|-------------|
| Apple Keychain | macOS | Touch ID |
| Windows Hello | Windows | Biometric |
| GNOME Keyring | Linux | Polkit |
| 1Password | Cross-platform | App unlock |
| File | All | No auth |

## Security Model

All keystore operations require explicit user approval:

1. **Biometric**: Touch ID, Windows Hello, fingerprint
2. **Polkit**: Linux password/fingerprint prompt
3. **1Password**: App unlock via biometric/password
4. **File**: No protection (for testing only)

## Usage

```cpp
#include <dashpay/keystore.hpp>

using namespace dashpay;

// Get default keystore for platform
auto keystore = Keystore::default_platform();

// Check if key exists
if (keystore->exists("my-account")) {
    auto key = keystore->get("my-account");
    // Use key for signing...
}

// Store a new key
keystore->store("new-account", keypair_bytes);
```

## Key Format

Keys are stored as 64-byte ed25519 keypairs:
- Bytes 0-31: Secret key
- Bytes 32-63: Public key

## Account Identifiers

```
keychain:account-name      # macOS Keychain
gnome-keyring:name       # Linux GNOME Keyring
windows-hello:name        # Windows Hello
1password:name            # 1Password
path/to/file.json          # File-based
```
