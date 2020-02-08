# Metering and Accounting

## Overview

DashPay includes server-side metering and accounting for tracking API usage and costs.

## Concepts

### Metering

Track usage metrics per API endpoint:
- Request count
- Response count
- Error rate
- Latency tracking

### Accounting

Track financial metrics:
- Revenue by endpoint
- Cost by protocol
- Settlement tracking

## Configuration

```yaml
metering:
  enabled: true
  sample_rate: 0.1  # Sample 10% of requests

accounting:
  enabled: true
  settlement_interval: 3600  # Seconds
```

## Metrics

| Metric | Description |
|--------|-------------|
| request_count | Total requests to endpoint |
| error_count | Failed requests |
| revenue_earned | USDC received |
| cost_incurred | USDC paid out |

## Implementation

```cpp
#include <dashpay/server/metering.hpp>

using namespace dashpay;

auto meter = Metering::create();
auto accounting = Accounting::create();

// Track a request
meter->record_request("/api/data", "GET");

// Track revenue
accounting->record_payment(1000000, "USDC");

// Get summary
auto summary = accounting->get_summary();
std::cout << "Revenue: " << summary.total_revenue << std::endl;
```
