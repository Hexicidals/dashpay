# Quick Start Guide

## Installation

```sh
npm install -g @dashpay/cli
```

Or build from source:

```sh
git clone https://github.com/dashpay-org/dashpay.git
cd dashpay
mkdir build && cd build
cmake .. && make
sudo make install
```

## Initial Setup

1. **Create your first wallet**:

```sh
dashpay setup
```

Follow the prompts to:
- Choose keystore (Touch ID, Windows Hello, or file)
- Set account name
- Select network (default: mainnet)

2. **Verify your account**:

```sh
dashpay whoami
```

## Making Your First Payment

### Using the CLI

```sh
# Send 0.1 USDC
dashpay send <recipient-address> 10000000
```

### Using curl

```sh
# curl will automatically retry with payment on 402
dashpay curl https://api.example.com/data
```

### Using Claude Code

```sh
# DashPay integration handles payments automatically
dashpay claude
```

## Understanding the Flow

```
Your Request → 402 Response
                ↓
            DashPay detects protocol (MPP or x402)
                ↓
            Prepare payment transaction
                ↓
            Request biometric approval
                ↓
            Sign transaction locally
                ↓
            Submit proof with request
                ↓
        Access granted
```

## Next Steps

- Read [Payment API Documentation](api.md)
- Explore [MPP Protocol](mpp.md)
- Learn about [x402 Protocol](x402.md)
- Review [Security Best Practices](security.md)
- Configure your [CLI Environment](cli.md)
