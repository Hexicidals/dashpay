# Model Context Protocol (MCP) Integration

## Overview

DashPay includes a built-in MCP server that allows AI assistants (Claude, Codex, etc.) to request paid API calls through the same wallet approval flow.

## Architecture

```
┌─────────────────────────────────────┐
│                                  │
│   AI Assistant (Claude/Codex)    │
│           ↓                    │
│   MCP Protocol (stdio)         │
│           ↓                    │
│   DashPay CLI                │
│           ↓                    │
│   Payment Handler               │
│           ↓                    │
│   Biometric Approval (Touch ID)  │
│           ↓                    │
│   Signed Transaction           │
│           ↓                    │
│   API Call (with payment)       │
│                                  │
└─────────────────────────────────────┘
```

## Available Tools

### curl

Execute HTTP requests with automatic payment handling.

### fetch

Built-in HTTP client for making requests.

### topup

Generate QR codes for funding your account.

## Usage

Start DashPay with MCP enabled:

```sh
dashpay --mcp
```

DashPay will detect the MCP connection and expose its tools.

## Tool Schema

### curl

```json
{
    "name": "curl",
    "description": "Execute curl with automatic 402 payment handling",
    "inputSchema": {
        "type": "object",
        "properties": {
            "url": {"type": "string"},
            "method": {"type": "string"},
            "headers": {"type": "array", "items": {"type": "string"}},
            "body": {"type": "string"}
        }
    }
}
```

### fetch

```json
{
    "name": "fetch",
    "description": "Fetch data from URL with automatic 402 payment handling",
    "inputSchema": {
        "type": "object",
        "properties": {
            "url": {"type": "string"}
        }
    }
}
```

### topup

```json
{
    "name": "topup",
    "description": "Generate QR code to fund DashPay account",
    "inputSchema": {
        "type": "object",
        "properties": {
            "amount": {"type": "string"},
            "method": {"type": "string", "enum": ["mobile_wallet", "onramp"]},
            "onramp": {"type": "string", "enum": ["coinbase", "paypal", "venmo"]}
        }
    }
}
```
