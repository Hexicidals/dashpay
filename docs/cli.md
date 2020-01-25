# CLI Documentation

## Commands

### setup

Initialize a new wallet account.

```sh
dashpay setup
```

Prompts for:
- Keystore backend (Apple Keychain, Windows Hello, GNOME Keyring, 1Password, File)
- Network (mainnet, devnet, localnet)
- Account name

### whoami

Display current account information.

```sh
dashpay whoami
```

Output:
```
Account: default
Public Key: 7xKXtg2MN87s8y...
Keystore: apple-keychain
Network: mainnet
```

### send

Send a payment.

```sh
dashpay send <recipient> <amount> [--reference REF]
```

Options:
- `--reference, -r`: Reference for transaction identification
- `--memo, -m`: Transaction memo
- `--network, -n`: Network to use (default: mainnet)

### balance

Check account balance.

```sh
dashpay balance
```

### topup

Generate a QR code for funding.

```sh
dashpay topup [--amount USDC] [--method mobile_wallet|onramp]
```

### server

Start payment gateway server.

```sh
dashpay server start [--spec spec.yml] [--debugger]
```

Options:
- `--spec`: API specification file
- `--debugger`: Enable payment debugger UI
- `--port`: Server port (default: 8899)

### claude

Run Claude Code with DashPay integration.

```sh
dashpay claude [args...]
```

### codex

Run Codex with DashPay integration.

```sh
dashpay codex [args...]
```

### curl

Wrap curl with payment handling.

```sh
dashpay curl [curl-args...]
```

### httpie

Wrap HTTPie with payment handling.

```sh
dashpay httpie [httpie-args...]
```

### wget

Wrap wget with payment handling.

```sh
dashpay wget [wget-args...]
```

## Global Options

- `--sandbox, -s`: Use sandbox environment
- `--verbose, -v`: Verbose output
- `--quiet, -q`: Quiet output
- `--config, -c`: Config file path
