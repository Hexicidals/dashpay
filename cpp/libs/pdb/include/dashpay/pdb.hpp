#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "dashpay/error.hpp"

namespace dashpay::pdb {

enum class PdbEvent : uint8_t {
    RequestStart,
    HeadersSent,
    ChallengeReceived,
    PaymentApproved,
    ProofSubmitted,
    ResponseReceived,
    Error,
    Complete,
};

struct PdbRequest {
    std::string method;
    std::string url;
    std::vector<std::pair<std::string, std::string>> headers;
    std::string body;
    uint64_t timestamp_ms;
};

struct PdbResponse {
    int status_code;
    std::vector<std::pair<std::string, std::string>> headers;
    std::string body;
    uint64_t timestamp_ms;
    std::optional<std::string> error;
};

struct PdbSession {
    std::string id;
    std::vector<PdbRequest> requests;
    std::vector<PdbResponse> responses;
    std::string protocol; // "MPP" or "x402"
};

class PaymentDebugger {
public:
    PaymentDebugger();

    void start_session(std::string_view id);
    void record_request(const PdbRequest& request);
    void record_response(const PdbResponse& response);
    void end_session();

    [[nodiscard]] PdbSession* current_session() const;
    [[nodiscard]] std::string generate_sequence_diagram(const PdbSession& session) const;

    [[nodiscard]] std::string to_html(const PdbSession& session) const;
    [[nodiscard]] std::string to_mermaid(const PdbSession& session) const;

private:
    std::unique_ptr<PdbSession> current_session_;
    std::vector<std::unique_ptr<PdbSession>> history_;
};

class PdbServer {
public:
    explicit PdbServer(uint16_t port = 8890);

    void start();
    void stop();
    [[nodiscard]] bool is_running() const;

private:
    uint16_t port_;
    bool running_ = false;
};

} // namespace dashpay::pdb
