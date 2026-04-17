# Metering and Accounting

## Overview

DashPay includes server-side metering and accounting for tracking API usage and costs.

## Metrics

### Request Metrics
- Request count
- Response count  
- Error rate
- Average latency
- P95 latency

### Accounting Metrics
- Revenue per endpoint
- Revenue per user
- Cost per endpoint (Solana fees)
- Total revenue
- Net profit

## Configuration

```yaml
metering:
  enabled: true
  sample_rate: 0.1  # Sample 10% of requests
  retention_days: 90  # Keep data for 90 days

accounting:
  enabled: true
  currency: DASH
  settlement_interval: 3600  # Settle every hour
```

## Implementation

```cpp
class Meter {
public:
    static void record_request(const std::string& endpoint, int status_code);
    static void record_payment(uint64_t lamports);
    
    static std::map<std::string, RequestMetrics> get_metrics();
    static std::map<std::string, AccountingMetrics> get_accounting();
};
```

## Usage

```bash
# Enable metering
dashpay server start --enable-metering

# Check metrics
dashpay metrics --endpoint api/quote --period 24h
```

## Privacy

All metering data is stored locally. No personal or identifiable information is sent to external services without user consent.
