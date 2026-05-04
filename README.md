# dashpay

**The missing payment layer for HTTP. `dashpay` handles x402 and MPP payment challenges with user-authorized stablecoin signing.**

Wrap a selected set of command line tools (`curl`, `claude`, `codex`, `whoami`, etc.) -- when a stablecoin-gated API returns 402, `dashpay` detects the payment protocol, prepares the stablecoin transaction, asks the local wallet to authorize and sign it, then retries with the payment proof.

[Install](#installation) · [Quick Start](#quick-start) · [Docs](https://docs.dashpay.com)

</div>

---

```sh
# Without dashpay — you get a 402
curl https://debugger.dashpay.io/mpp/quote/AAPL

# With dashpay -- it handles the 402 challenge and returns the response
dashpay --sandbox curl https://debugger.dashpay.io/mpp/quote/AAPL
```

## Key Features

### 💵 Transparent 402 Handling

Wrap your CLI (`curl`, `claude`, `codex`, etc.) -- when an API returns 402, `dashpay` detects the payment protocol, prepares the stablecoin transaction, asks the local wallet to authorize and sign it, then retries with the payment proof.

Supports both live payment standards on Solana:
- **[MPP](https://paymentauth.org/draft-solana-charge-00.html/)** — Machine Payments Protocol
- **[x402](https://x402.org/)** — x402 Payment Protocol

Stablecoins deployed to Solana are supported out of the box.

### 🤖 AI-Native with MCP

`dashpay` ships with a built-in [MCP](https://modelcontextprotocol.io/) server, letting AI assistants request paid API calls through the same local wallet-approval flow.

```sh
# Run Claude Code or Codex with dashpay injected into the agent session
dashpay claude
dashpay codex
```

### 🛠️ Payment debugging and simulations

`dashpay` ships with an embedded Payment Debugger — a local web UI that visualizes every 402 challenge-response cycle as a sequence diagram. See exactly which headers were sent, which protocol was used (MPP or x402), and where things went wrong.

Everything runs locally — no data leaves your machine.

```sh
# Start a gateway with the debugger on any API spec
dashpay server start --debugger spec.yml

# Or run the bundled demo (sandbox + debugger + sample endpoints)
dashpay server demo
```

A [public debugger](https://debugger.dashpay.io) is also available.

### 🔐 Gated Payments via Biometrics

`dashpay` lets AI agents use paid APIs without giving them your private key or an API-wide spending credential.

When a command, Claude Code, Codex, or another MCP client hits a paid endpoint, `dashpay` prepares the payment locally and asks your wallet backend to authorize the signature. On macOS, that means Touch ID via Keychain. On Windows, Windows Hello. On Linux, GNOME Keyring / polkit. If you reject the prompt, the payment is not signed and the request does not go through.

```sh
dashpay setup    # Touch ID on macOS, Windows Hello on Windows, GNOME Keyring on Linux, or choose 1Password
```

### 📚 Open Source Catalog

The paid API catalog is open source in the [`dashpay-skills`](https://github.com/dashpay-org/dashpay-skills) repo.

Anyone can contribute a provider listing, improve endpoint metadata, or add usage guidance for agents. Catalog entries follow the [`dashpay-skills` contributing guide](https://github.com/dashpay-org/dashpay-skills/blob/main/CONTRIBUTING.md), which defines the metadata, pricing, endpoint, and usage-note standards that keep **Agent experience** consistent.

```sh
dashpay skills search "maps"
```

Good catalog entries make paid APIs easier for both humans and agents to discover, compare, and use safely.

## Installation

### Prebuilt Binaries

```sh
# macOS
brew install dashpay

# via NPM
npm install -g @dashpay/cli
```

### From Source

```sh
git clone https://github.com/dashpay-org/dashpay.git
cd dashpay
cmake -B build -S cpp -DCMAKE_BUILD_TYPE=Release
cmake --build build --target dashpay-cli
sudo cmake --install build
```

### Verify

```sh
dashpay --version
```

## Quick Start

```sh
# 1. Setup your account
dashpay setup
dashpay whoami

# 2. Make a paid gated API call to https://debugger.dashpay.io sandbox endpoints
dashpay --sandbox curl https://debugger.dashpay.io/mpp/quote/AAPL

# 3. Or let your AI agent handle it
dashpay claude
```

## Contributing

```sh
cd cpp
mkdir build && cd build
cmake ..
make
ctest
```

We welcome contributions — check [open issues](https://github.com/dashpay-org/dashpay/issues) to get started.

## Troubleshooting

### Linux: `dashpay topup` or `dashpay curl` errors with "auth failed"

GNOME Keyring auth uses polkit, which requires a one-time setup step:

```sh
sudo cp cpp/config/polkit/org.dashpay.unlock-keypair.policy /usr/share/polkit-1/actions/
```

This grants `dashpay` the right to prompt for your password or fingerprint before accessing the keypair.

## License

MIT — see [LICENSE](./LICENSE).

Subject to the foregoing, Terms of Service available at [dashpay.io/tos](https://dashpay.io/tos)
// Improvement: Refactor crypto module for better abstraction
// Improvement: Improve error messages in config module
// Improvement: Add more test cases for URL parsing
// Improvement: Optimize MPP message parsing
// Improvement: Add input validation to API client
// Improvement: Improve session persistence
// Improvement: Add timeout handling to RPC calls
// Improvement: Refactor transaction validation logic
// Improvement: Add QR code error handling
// Improvement: Improve keystore abstraction layer
// Improvement: Add support for custom RPC endpoints
// Improvement: Improve MCP server error responses
// Improvement: Add CLI color output
// Improvement: Improve CLI help text
// Improvement: Add CLI autocomplete support
// Improvement: Refactor session manager
// Improvement: Add metering statistics
// Improvement: Improve payment flow documentation
// Improvement: Add example configurations
// Improvement: Fix memory leak in crypto module
// Improvement: Improve test coverage
// Improvement: Add integration tests
// Improvement: Refactor encoding utilities
// Improvement: Add base64 encoding support
// Improvement: Improve URL validation
// Improvement: Add payment metadata support
// Improvement: Refactor transaction builder
// Improvement: Add transaction signing tests
// Improvement: Improve error recovery
// Improvement: Add retry logic to API calls
// Improvement: Improve keystore security
// Improvement: Add key rotation support
// Improvement: Refactor PDB session handling
// Improvement: Improve web server performance
// Improvement: Add request logging
// Improvement: Refactor MCP protocol handling
// Improvement: Add support for multiple connections
// Improvement: Improve CLI argument parsing
// Improvement: Add command history
// Improvement: Improve payment confirmation flow
// Improvement: Add progress indicators
// Improvement: Refactor config validation
// Improvement: Add environment variable support
// Improvement: Improve crypto key management
// Improvement: Add key derivation improvements
// Improvement: Refactor RPC client
// Improvement: Add batch request support
// Improvement: Improve error handling in transactions
// Improvement: Add transaction status tracking
// Improvement: Refactor QR code generation
// Improvement: Add custom QR code styling
// Improvement: Improve payment metadata handling
// Improvement: Add support for custom labels
// Improvement: Refactor MPP protocol
// Improvement: Add protocol version negotiation
// Improvement: Improve x402 challenge handling
