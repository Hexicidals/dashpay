#include <dashpay/pdb.hpp>
#include <format>
#include <random>
#include <sstream>

#include "dashpay/crypto.hpp"
#include "dashpay/error.hpp"

namespace dashpay::pdb {

PaymentDebugger::PaymentDebugger()
    : current_session_(nullptr)
    , history_()
{
}

void PaymentDebugger::start_session(std::string_view id) {
    std::lock_guard<std::mutex> lock(session_mutex_);

    current_session_ = std::make_unique<PdbSession>();
    current_session_->id = std::string(id);
    current_session_->protocol = "MPP"; // Default
    current_session_->created_at = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

void PaymentDebugger::record_request(const PdbRequest& request) {
    std::lock_guard<std::mutex> lock(session_mutex_);

    if (!current_session_) {
        return;
    }

    current_session_->requests.push_back(request);
}

void PaymentDebugger::record_response(const PdbResponse& response) {
    std::lock_guard<std::mutex> lock(session_mutex_);

    if (!current_session_) {
        return;
    }

    current_session_->responses.push_back(response);

    // Detect protocol from response
    if (response.status_code == 402) {
        for (const auto& [name, value] : response.headers) {
            if (name == "x402-payment" || name == "x402-challenge") {
                current_session_->protocol = "x402";
                break;
            } else if (name.find("MPP") != std::string::npos) {
                current_session_->protocol = "MPP";
                break;
            }
        }
    }
}

void PaymentDebugger::end_session() {
    std::lock_guard<std::mutex> lock(session_mutex_);

    if (current_session_) {
        history_.push_back(std::move(current_session_));
        current_session_ = nullptr;
    }
}

PdbSession* PaymentDebugger::current_session() const {
    return current_session_.get();
}

std::string PaymentDebugger::generate_sequence_diagram(const PdbSession& session) const {
    std::ostringstream diagram;
    diagram << "sequenceDiagram\n";
    diagram << "    participant C as Client\n";
    diagram << "    participant D as DashPay\n";
    diagram << "    participant K as Keystore\n";
    diagram << "    participant S as Server\n\n";

    size_t req_idx = 0;
    size_t resp_idx = 0;

    // Generate sequence from requests and responses
    while (req_idx < session.requests.size() || resp_idx < session.responses.size()) {
        bool is_request = (req_idx < session.requests.size() &&
                             (resp_idx == session.responses.size() ||
                              session.responses[resp_idx].timestamp_ms > session.requests[req_idx].timestamp_ms));

        if (is_request) {
            const auto& req = session.requests[req_idx];

            diagram << "    C->>S: Make " << req.method << " " << req.url << "\n";

            if (req_idx > 0) {
                diagram << "    S->>C: Response " << req_idx << " (" << session.requests[req_idx - 1].status_code << ")\n";
            }

            ++req_idx;
        } else {
            const auto& resp = session.responses[resp_idx];

            if (resp.status_code == 402) {
                diagram << "    S->>D: 402 Challenge (" << session.protocol << ")\n";
            } else {
                diagram << "    S->>D: Response " << resp.status_code << "\n";
            }

            ++resp_idx;
        }
    }

    diagram << "end\n";
    return diagram.str();
}

std::string PaymentDebugger::to_html(const PdbSession& session) const {
    std::ostringstream html;

    html << R"(<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>DashPay Payment Debugger - )" << session.id << R"(</title>
    <style>
        body { font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif; margin: 20px; background: #0d1117; color: #333; }
        .container { max-width: 1200px; margin: 0 auto; }
        .header { background: #1e88e5; color: white; padding: 15px 20px; border-radius: 8px; margin-bottom: 20px; }
        .header h1 { margin: 0; font-size: 24px; }
        .session-info { display: grid; grid-template-columns: auto auto; gap: 10px; font-size: 14px; }
        .sequence-diagram { background: white; border-radius: 8px; padding: 20px; overflow-x: auto; }
        .request-list, .response-list { display: grid; grid-template-columns: 1fr; gap: 8px; }
        .item { background: #f5f5f5; padding: 10px; border-radius: 4px; font-size: 12px; }
        .item.request { background: #e3f2fd; border-left: 3px solid #1e88e5; }
        .item.response { background: #dcfce7; border-left: 3px solid #059662; }
        .method { font-weight: 600; color: #666; }
        .timestamp { color: #999; font-size: 11px; }
        .badge { display: inline-block; padding: 2px 8px; border-radius: 12px; font-size: 11px; font-weight: 500; }
        .badge.mpp { background: #14f195; color: white; }
        .badge.x402 { background: #4f46e5; color: white; }
        .challenge-box { background: #1a1a1a; color: #333; padding: 15px; border-radius: 4px; margin: 10px 0; font-family: monospace; font-size: 12px; overflow-x: auto; }
        pre { white-space: pre-wrap; margin: 0; }
        .footer { margin-top: 30px; text-align: center; color: #999; font-size: 12px; }
        .protocol-mpp { color: #14f195; }
        .protocol-x402 { color: #4f46e5; }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>DashPay Payment Debugger</h1>
            <div class="session-info">
                <div><strong>Session ID:</strong> )" << session.id << R"(</div>
                <div><strong>Protocol:</strong> <span class="badge )" << session.protocol << R"()">)" << session.protocol << R"(</span></div>
                <div><strong>Requests:</strong> )" << session.requests.size() << R"(</div>
                <div><strong>Responses:</strong> )" << session.responses.size() << R"(</div>
                <div><strong>Created:</strong> )" << std::format("{:%H:%M}", session.created_at) << R"(</div>
            </div>
        </div>

        <div class="sequence-diagram">
            <h2>Sequence Diagram</h2>
)";

    // Generate request/response lists
    html << R"        <h3>Requests</h3>
        html << "        <div class=\"request-list\">\n";

    for (const auto& req : session.requests) {
        html << "            <div class=\"item request\">\n";
        html << "                <div class=\"timestamp\">" << std::format("{:%H:%M}", req.timestamp_ms) << R"(</div>\n";
        html << "                <div class=\"method\">" << req.method << R"(</div>\n";
        html << "                <div>" << req.url << R"(</div>\n";

        if (!req.headers.empty()) {
            html << "                <details><summary>Headers (" << req.headers.size() << R"(</summary>\n";
            for (const auto& [name, value] : req.headers) {
                html << "                    <div><strong>" << name << R"(:</strong> " << value << R"(</div>\n";
            }
            html << "                </details>\n";
        }

        html << "            </div>\n";
    }

    html << "        </div>\n";

    html << R"        <h3>Responses</h3>
        html << "        <div class=\"response-list\">\n";

    for (const auto& resp : session.responses) {
        std::string status_class = resp.status_code == 200 ? "response" : "error";
        html << "            <div class=\"item " << status_class << R"\">\n";
        html << "                <div class=\"timestamp\">" << std::format("{:%H:%M}", resp.timestamp_ms) << R"(</div>\n";
        html << "                <div class=\"badge\">" << resp.status_code << R"(</div>\n";

        if (!resp.headers.empty()) {
            html << "                <details><summary>Headers (" << resp.headers.size() << R"(</summary>\n";
            for (const auto& [name, value] : resp.headers) {
                html << "                    <div><strong>" << name << R"(:</strong> " << value << R"(</div>\n";
            }
            html << "                </details>\n";
        }

        if (!resp.body.empty()) {
            html << "                <div class=\"challenge-box\"><pre>" << resp.body << R"(</pre></div>\n";
        }

        html << "            </div>\n";
    }

    html << "        </div>\n";

    html << R"        <div class=\"footer\">Payment Debugger v0.16.0 - DashPay</div>\n";
    html << "    </div>\n";
    html << "</body>\n";
    html << "</html>";

    return html.str();
}

std::string PaymentDebugger::to_mermaid(const PdbSession& session) const {
    std::ostringstream mermaid;

    mermaid << "sequenceDiagram\n";
    mermaid << "    C->>S: Make " << session.requests.front().method << " " << session.requests.front().url << "\n";

    for (size_t i = 1; i < session.requests.size(); ++i) {
        mermaid << "    S->>C: Response " << i << " (" << session.responses[i - 1].status_code << ")\n";
    }

    mermaid << "    S->>D: 402 Challenge (" << session.protocol << ")\n";

    for (size_t i = 0; i < session.responses.size(); ++i) {
        mermaid << "    D->>K: Get Signature\n";
        mermaid << "    K->>D: Sign Transaction\n";
        mermaid << "    D->>S: Submit Proof\n";
        mermaid << "    S->>D: Response " << session.responses[i].status_code << "\n";
        if (i < session.responses.size() - 1) {
            mermaid << "    C->>S: Make Request " << session.requests[i + 1].method << "\n";
            mermaid << "    S->>D: 402 Challenge (" << session.protocol << ")\n";
        }
    }

    mermaid << "end\n";

    return mermaid.str();
}

PdbServer::PdbServer(uint16_t port)
    : port_(port)
    , running_(false)
{
}

void PdbServer::start() {
    running_ = true;
    // In production, would start web server on port_
}

void PdbServer::stop() {
    running_ = false;
}

bool PdbServer::is_running() const {
    return running_;
}

} // namespace dashpay::pdb
