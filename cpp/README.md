# C++ Implementation

## Directory Structure

```
cpp/
├── apps/
│   └── cli/              # Command-line interface
├── libs/
│   ├── core/              # Core payment logic
│   ├── keystore/           # Key storage and signing
│   ├── types/              # Shared types and utilities
│   ├── mcp/                # Model Context Protocol server
│   └── pdb/                # Payment Debugger
├── tests/                   # Test suite
└── CMakeLists.txt           # Root CMake configuration
```

## Core Library (libs/core)

Core payment functionality including:
- Configuration management
- Account registry
- Payment API
- Cryptographic primitives
- Transaction encoding/decoding
- URL encoding
- QR code generation
- Persistence layer

## Keystore Library (libs/keystore)

Secure key storage with platform-specific backends:
- **macOS**: Apple Keychain with Touch ID
- **Windows**: Windows Hello
- **Linux**: GNOME Keyring
- **Cross-platform**: 1Password integration
- **File-based**: Local JSON keypairs

## Types Library (libs/types)

Shared utilities and types:
- Network configuration
- Amount conversions
- Pagination helpers
- Common data structures

## CLI (apps/cli)

Command-line interface with commands:
- `setup` - Initialize wallet
- `whoami` - Show current account
- `send` - Send payment
- `balance` - Check balance
- `topup` - Fund account
- `server` - Run payment gateway
- `claude` - Run with Claude Code
- `codex` - Run with Codex

## Building

```sh
mkdir build && cd build
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release
```

## Testing

```sh
ctest --test-dir build
```
