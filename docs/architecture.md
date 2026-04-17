# DashPay Architecture

## Overview

DashPay is a payment layer for HTTP built in C++ that enables automatic payment handling for 402-status responses.

## Components

### Core Library (`libs/core/`)
- **Error Handling**: `error.hpp`, `error.cpp`
- **Configuration**: `config.hpp`, `config.cpp`
- **Crypto**: `crypto.hpp`, `crypto.cpp` (OpenSSL Ed25519)
- **Encoding**: `encoding.hpp` (Base64, URI encoding)
- **Payment API**: `api.hpp`, `api.cpp`
- **Payment Protocols**: `mpp.hpp`, `x402.hpp`, `mpp.cpp`, `x402.cpp`
- **RPC Client**: `rpc.hpp`, `rpc.cpp` (libcurl)
- **Transaction**: `transaction.hpp`
- **Transaction Validation**: `transaction_validation.hpp`, `transaction_validation.cpp`
- **Types**: `types.hpp`, `types.hpp` (shared)
- **URL**: `url.hpp`, `url.cpp`

### Keystore (`libs/keystore/`)
- **Apple Keychain**: `apple_keychain.cpp` (macOS Touch ID)
- **Windows Hello**: `windows_hello.cpp` (Windows Hello)
- **GNOME Keyring**: `gnome_keyring.cpp` (Linux)
- **1Password**: `onepassword.cpp` (Cross-platform)

### CLI (`apps/cli/`)
- **Main Application**: `main.cpp`
- **Command Interface**: `cli.hpp`
- **Commands**: `commands.hpp` (setup, whoami, send, balance, topup, server, curl, httpie, wget)
- **Wallet Adapters**: Integration with Phantom, Solflare, Trust, Ledger, Trezor

### MCP Server (`libs/mcp/`)
- **Server Interface**: `mcp/src/server.cpp`
- **Tool Registry**: Dynamic tool registration
- **Request/Response**: JSON-RPC for MCP

### Payment Debugger (`libs/pdb/`)
- **Session Management**: `pdb/src/session.cpp`
- **UI Generation**: HTML sequence diagrams
- **Protocol Detection**: MPP vs x402

## Data Flow

```
┌─────────────┐
│  CLI Commands │
│  └─────┤      ┌───────┐
│        Payment API          │      │   Keystore   │
│             │            │      │  └─────────┤
│   ┌────┴───┐            │              │
│   │      RPC Client         │              │
│   │   │         │              │
│   └──────┴───┘            │              │
│         MPP / x402            │              │
│             │            │              │
│   └────────────────────────────────┘
│              │              │
│      ┌──────────────────┐
│      │   Payment Debugger (PDB)   │
│      └─────────────────────┘
└─────────────────────────────────────────┘
```

## Technology Stack

- **Language**: C++20
- **Build System**: CMake 3.20+
- **Cryptography**: OpenSSL (Ed25519, SHA-512)
- **HTTP Client**: libcurl
- **Async Runtime**: std::thread
- **Platform Support**: Apple Foundation, Windows Credential Manager, libsecret

## Security

- **Key Storage**: Platform-native secure vaults (Keychain, DPAPI, Secret Service)
- **Authentication**: Biometric (Touch ID, Windows Hello, Fingerprint)
- **Encryption**: Ed25519 signatures
- **No Secrets in Code**: All sensitive data in system vaults
- **Zero-Trust**: User approval required for each payment
