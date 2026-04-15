#include <dashpay/metering.hpp>
#include <dashpay/error.hpp>
#include <dashpay/types.hpp>

namespace dashpay::metering {

Meter::Meter() {}

void Meter::record_request(const RequestMetrics& metrics) {
    std::lock_guard<std::mutex> lock(mutex_);

    metrics.timestamp_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();

    auto request_data = get_request_metrics(metrics.method);

    // Store or update metrics for endpoint
    requests_[metrics.url].total_requests++;
    requests_[metrics.url].total_lamports_sent += request_data.lamports;

    std::cout << "METER: " << metrics.method << " " << metrics.url
              << " LAMPORTS:" << request_data.lamports << std::endl;
}

void Meter::record_response(const ResponseMetrics& metrics) {
    std::lock_guard<std::mutex> lock(mutex_);

    metrics.timestamp_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();

    auto response_data = get_response_metrics(metrics.status_code);

    // Update metrics for endpoint
    responses_[metrics.url].total_responses++;
    responses_[metrics.url].total_lamports_received += response_data.lamports;
    responses_[metrics.url].total_latency += metrics.latency_ms;

    std::cout << "METER: response " << metrics.status_code << " " << metrics.url
              << " LAMPORTS:" << response_data.lamports
              << " LATENCY:" << metrics.latency_ms << "ms" << std::endl;
}

RequestMetrics Meter::get_request_metrics(std::string_view endpoint) const {
    RequestMetrics metrics;
    metrics.method = std::string(endpoint);
    metrics.url = std::string(endpoint);
    metrics.timestamp_ms = 0;
    metrics.status_code = 0;
    metrics.latency_ms = 0;

    // Try to load from storage
    auto endpoint_data = requests_.find(endpoint);
    if (endpoint_data != requests_.end()) {
        metrics.total_requests = endpoint_data->total_requests;
        metrics.total_lamports_sent = endpoint_data->total_lamports_sent;
    }

    return metrics;
}

ResponseMetrics Meter::get_response_metrics(std::string_view endpoint) const {
    ResponseMetrics metrics;
    metrics.status_code = 200;
    metrics.timestamp_ms = 0;
    metrics.latency_ms = 0;

    // Try to load from storage
    auto endpoint_data = responses_.find(endpoint);
    if (endpoint_data != responses_.end()) {
        metrics.total_responses = endpoint_data->total_responses;
        metrics.total_lamports_received = endpoint_data->total_lamports_received;
        metrics.total_latency = endpoint_data->total_latency;
    }

    return metrics;
}

void Meter::reset() {
    std::lock_guard<std::mutex> lock(mutex_);

    requests_.clear();
    responses_.clear();

    std::cout << "METER: reset" << std::endl;
}

std::vector<RequestMetrics> Meter::get_all_requests() const {
    std::lock_guard<std::mutex> lock(mutex_);

    std::vector<RequestMetrics> all;
    all.reserve(requests_.size());

    for (const auto& [url, metrics] : requests_) {
        all.push_back(metrics);
    }

    return all;
}

std::vector<ResponseMetrics> Meter::get_all_responses() const {
    std::lock_guard<std::mutex> lock(mutex_);

    std::vector<ResponseMetrics> all;
    all.reserve(responses_.size());

    for (const auto& [url, metrics] : responses_) {
        all.push_back(metrics);
    }

    return all;
}

void Meter::update_sample_rate() {
    std::lock_guard<std::mutex> lock(mutex_);

    sample_rate_ = (request_count_ + response_count_) / (elapsed_ms_ / 1000);
}

} // namespace dashpay::metering
