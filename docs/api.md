# DashPay API

## REST API Reference

### Endpoints

#### Authentication
```
POST /api/v1/auth/register
POST /api/v1/auth/login
POST /api/v1/auth/logout
```

#### Payments
```
POST /api/v1/payments/create
GET  /api/v1/payments/:id
GET /api/v1/payments/:id/status
POST /api/v1/payments/confirm
```

#### Transactions
```
POST /api/v1/transactions
GET /api/v1/transactions
POST /api/v1/transactions/:id
```

#### Accounts
```
GET /api/v1/accounts
POST /api/v1/accounts/:id
```

### Request Headers

```
Authorization: Bearer <token>
Content-Type: application/json
x402-payment: <challenge-base64>
x402-proof: <proof-base64>
x402-status: <status>
```

### Response Codes

| Code | Description |
|-------|-------------|
| 200 | Success |
| 402 | Payment Required |
| 401 | Unauthorized |
| 400 | Bad Request |
| 500 | Server Error |

### Pagination

```
GET /api/v1/endpoint?page=1&per_page=50
```

### Rate Limiting

Default: 100 requests/minute
Authenticated: 1000 requests/minute
