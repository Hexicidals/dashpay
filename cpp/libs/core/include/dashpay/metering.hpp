#pragma once

#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "dashpay/error.hpp"

namespace dashpay::metering {

struct RequestMetrics {
    uint64_t timestamp_ms;
    std::string method;
    std::string url;
    int status_code;
    uint64_t latency_ms;
};

struct ResponseMetrics {
    uint64_t timestamp_ms;
    int status_code;
    uint64_t latency_ms;
};

class Meter {
public:
    Meter();

    void record_request(const RequestMetrics& metrics);
    void record_response(const ResponseMetrics& metrics);

    [[nodiscard]] RequestMetrics get_request_metrics(std::string_view endpoint) const;
    [[nodiscard]] ResponseMetrics get_response_metrics(std::string_view endpoint) const;

    [[nodiscard]] std::vector<RequestMetrics> get_all_requests() const;
    [[nodiscard]] std::vector<ResponseMetrics> get_all_responses() const;

    void reset();

private:
    std::mutex mutex_;
    std::vector<RequestMetrics> requests_;
    std::vector<ResponseMetrics> responses_;
    size_t sample_rate = 0;

    void update_sample_rate();
};

class Accounting {
public:
    Accounting();

    void record_payment(uint64_t lamports, std::string_view endpoint);
    void record_settlement(uint64_t lamports, std::string_view reference);

    [[nodiscard]] AccountingMetrics get_accounting_metrics(std::string_view user) const;
    [[nodiscard]] AccountingMetrics get_endpoint_metrics(std::string_view endpoint) const;

private:
    std::mutex mutex_;
    std::map<std::string, uint64_t> user_revenue_;
    std::map<std::string, uint64_t> endpoint_cost_;

    uint64_t total_lamports_sent_ = 0;
    uint64_t total_fees_paid_ = 0;
};

struct AccountingMetrics {
    std::string user;
    uint64_t total_lamports_sent;
    uint64_t total_fees_paid;
    double net_profit;
};

struct MeteringConfig {
    bool enabled = true;
    double sample_rate = 0.1; // 10% of requests
    uint32_t retention_days = 90;
};

} // namespace dashpay::metering
