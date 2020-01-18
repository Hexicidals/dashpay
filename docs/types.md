# Core Types Documentation

## Network

```cpp
enum class Network : uint8_t {
    Mainnet,
    Devnet,
    Testnet,
    Localnet,
    Sandbox,
};
```

| Network | RPC URL |
|---------|----------|
| Mainnet | `https://api.dash.org` |
| Devnet | `https://api.devnet.dash.org` |
| Testnet | `https://api.testnet.dash.org` |
| Localnet | `http://localhost:8899` |
| Sandbox | `https://402.surfnet.dev:8899` |

## TransactionStatus

```cpp
enum class TransactionStatus : uint8_t {
    Pending,
    Confirmed,
    Failed,
    Timeout,
};
```

## PaymentProtocol

```cpp
enum class PaymentProtocol : uint8_t {
    Mpp,
    X402,
};
```

## Amount

```cpp
struct Amount {
    uint64_t lamports;
    double dash;

    static Amount from_lamports(uint64_t value) noexcept;
    static Amount from_dash(double value) noexcept;
};
```

### Conversion

1 Dash = 100,000,000 lamports

```cpp
auto amount = Amount::from_dash(1.5);
std::cout << amount.lamports << " lamports" << std::endl;
```

## PaginatedResponse

```cpp
template <typename T>
struct PaginatedResponse {
    std::vector<T> items;
    uint64_t total;
    std::optional<std::string> next_cursor;
};
```
