# DashPay CLI

A payment layer for HTTP that handles 402-status responses with automatic payment processing.

## Features

- **Automatic Payment Detection**: Detects x402 and MPP payment challenges
- **Secure Key Storage**: Platform-specific keystores (Apple Keychain, Windows Hello, GNOME Keyring, 1Password)
- **Multiple Payment Protocols**: MPP (Machine Payments Protocol) and x402 support
- **Biometric Authentication**: Touch ID, Windows Hello, fingerprint
- **Transaction Management**: Create, validate, and track payments
- **Session Tracking**: Monitor payment flows with debugging capabilities
- **Model Context Protocol**: AI agent integration

## Installation

### Build from Source

```bash
# Install dependencies (Ubuntu/Debian)
sudo apt install build-essential cmake pkg-config libcurl4-openssl-dev

# Build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
sudo make install
```

### macOS (Homebrew)

```bash
brew install cmake pkg-config libcurl openssl
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(sysctl -n hw.ncpu)
sudo make install
```

### Windows

```powershell
# Install dependencies with vcpkg or Chocolatey
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
cmake --install . --config Release
```

## Usage

### Setup Wallet

```bash
dashpay setup
```

Select keystore backend and generate keypair.

### Check Balance

```bash
dashpay balance
```

### Send Payment

```bash
dashpay send 7xKXtg2MN87s8y... 10000000
```

### With Reference

```bash
dashpay send 7xKXtg2MN87s8y... 10000000 --reference order-123
```

### With Memo

```bash
dashpay send 7xKXtg2MN87s8y... 10000000 --memo "Thank you for your purchase"
```

### Using with curl

```bash
dashpay curl https://api.example.com/data
```

### Automatic Payments

The CLI automatically handles 402 responses:
- Detects x402 or MPP protocol from response headers
- Presents payment confirmation with details (amount, recipient)
- Waits for biometric approval (Touch ID, Windows Hello, etc.)
- Signs transaction locally (never sends keys to server)
- Submits payment proof and retries request

## Payment Protocols

### MPP (Machine Payments Protocol)

Off-chain payment layer for automatic payments:
- Charge creation with unique link keys
- Multi-hop payment support
- Reference-based transaction tracking
- Works with Dash stablecoin

### x402 Payment Protocol

Identity-based payment protocol:
- Challenge/response flow
- Ed25519 signature verification
- Compatible with multiple stablecoins
- Support for reference and memo fields

## Development

### Project Structure

```
dashpay/
├── cpp/                    # C++ implementation
│   ├── libs/
│   │   ├── core/          # Core library
│   │   ├── keystore/       # Key storage backends
│   │   ├── mcp/           # Model Context Protocol
│   │   └── pdb/           # Payment Debugger
│   └── apps/
│       └── cli/           # CLI application
├── docs/                   # Documentation
├── package.json            # NPM configuration
└── CMakeLists.txt          # Build system
```

## License

MIT License - Copyright (c) 2020-2023 DashPay

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

## Security

See [SECURITY.md](SECURITY.md) for security policies and best practices.

## Support

- GitHub Issues: https://github.com/Hexicidals/dashpay/issues
- Discussions: https://github.com/Hexicidals/dashpay/discussions
