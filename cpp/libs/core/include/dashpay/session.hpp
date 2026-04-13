#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "dashpay/error.hpp"

namespace dashpay {

enum class SessionStatus : uint8_t {
    Pending,
    Active,
    Approved,
    Rejected,
    Completed,
};

struct SessionConfig {
    std::string id;
    SessionStatus status = SessionStatus::Pending;
    std::string network;
    std::string account_name;
    uint64_t created_at_ms;
    uint64_t expires_at_ms;
    std::string reference;
    std::string memo;
    uint64_t total_lamports_sent = 0;
    uint64_t total_lamports_received = 0;
    std::vector<std::string> requests;
    std::vector<std::string> responses;
    std::optional<std::string> error_message;
    uint64_t last_activity_at_ms;
};

class SessionManager {
public:
    virtual ~SessionManager() = default;

    [[nodiscard]] virtual Result<SessionConfig> create_session(
        std::string_view network,
        std::string_view account_name,
        uint64_t ttl_seconds = 3600
    ) = 0;

    [[nodiscard]] virtual Result<SessionConfig> get_session(std::string_view session_id) = 0;
    [[nodiscard]] virtual Result<void> update_session(
        std::string_view session_id,
        const SessionUpdate& update
    ) = 0;

    [[nodiscard]] virtual Result<void> approve_session(std::string_view session_id) = 0;
    [[nodiscard]] virtual Result<void> reject_session(std::string_view session_id) = 0;
    [[nodiscard]] virtual Result<void> complete_session(std::string_view session_id) = 0;
    [[nodiscard]] virtual Result<void> set_session_status(
        std::string_view session_id,
        SessionStatus status
    ) = 0;

    [[nodiscard]] virtual Result<std::vector<SessionConfig>> list_sessions() = 0;

    [[nodiscard]] virtual Result<std::vector<std::pair<std::string_view, SessionConfig>>> get_active_sessions() = 0;
};

struct SessionUpdate {
    std::string reference;
    std::string memo;
    bool increment_lamports_sent = false;
    bool increment_lamports_received = false;
};

class InMemorySessionManager : public SessionManager {
public:
    InMemorySessionManager() = default;

    [[nodiscard]] Result<SessionConfig> create_session(
        std::string_view network,
        std::string_view account_name,
        uint64_t ttl_seconds = 3600
    ) override {
        SessionConfig session{
            .id = generate_session_id(),
            .status = SessionStatus::Active,
            .network = std::string(network),
            .account_name = std::string(account_name),
            .created_at_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()
            ).count(),
            .expires_at_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()
            ).count() + (ttl_seconds * 1000),
        };

        sessions_[session.id] = std::move(session);
        return session;
    }

    [[nodiscard]] Result<SessionConfig> get_session(std::string_view session_id) override {
        auto it = sessions_.find(session_id);
        if (it == sessions_.end()) {
            return Error(ErrorCode::SessionNotFound,
                        std::format("Session not found: {}", session_id));
        }

        return *it;
    }

    Result<void> update_session(
        std::string_view session_id,
        const SessionUpdate& update
    ) override {
        auto it = sessions_.find(session_id);
        if (it == sessions_.end()) {
            return Error(ErrorCode::SessionNotFound,
                        std::format("Session not found: {}", session_id));
        }

        if (update.reference) {
            it->reference = *update.reference;
        }
        if (update.memo) {
            it->memo = *update.memo;
        }
        if (update.increment_lamports_sent) {
            it->total_lamports_sent += 1;
        }
        if (update.increment_lamports_received) {
            it->total_lamports_received += 1;
        }
        it->last_activity_at_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
            ).count();

        return {};
    }

    Result<void> approve_session(std::string_view session_id) override {
        auto it = sessions_.find(session_id);
        if (it == sessions_.end()) {
            return Error(ErrorCode::SessionNotFound,
                        std::format("Session not found: {}", session_id));
        }

        it->status = SessionStatus::Approved;
        it->last_activity_at_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
            ).count();

        return {};
    }

    Result<void> reject_session(std::string_view session_id) override {
        auto it = sessions_.find(session_id);
        if (it == sessions_.end()) {
            return Error(ErrorCode::SessionNotFound,
                        std::format("Session not found: {}", session_id));
        }

        it->status = SessionStatus::Rejected;
        it->error_message = "Payment rejected by user";
        it->last_activity_at_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
            ).count();

        return {};
    }

    Result<void> complete_session(std::string_view session_id) override {
        auto it = sessions_.find(session_id);
        if (it == sessions_.end()) {
            return Error(ErrorCode::SessionNotFound,
                        std::format("Session not found: {}", session_id));
        }

        it->status = SessionStatus::Completed;
        it->last_activity_at_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
            ).count();

        return {};
    }

    Result<void> set_session_status(
        std::string_view session_id,
        SessionStatus status
    ) override {
        auto it = sessions_.find(session_id);
        if (it == sessions_.end()) {
            return Error(ErrorCode::SessionNotFound,
                        std::format("Session not found: {}", session_id));
        }

        it->status = status;
        it->last_activity_at_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
            ).count();

        return {};
    }

    [[nodiscard]] Result<std::vector<SessionConfig>> list_sessions() const override {
        std::vector<SessionConfig> all;
        for (const auto& [id, config] : sessions_) {
            all.push_back({id, config});
        }

        return all;
    }

    [[nodiscard]] Result<std::vector<std::pair<std::string_view, SessionConfig>>> get_active_sessions() const override {
        std::vector<std::pair<std::string_view, SessionConfig>> active;

        for (const auto& [id, config] : sessions_) {
            if (config->status == SessionStatus::Active || config->status == SessionStatus::Approved || config->status == SessionStatus::Pending) {
                active.push_back({id, config});
            }
        }

        return active;
    }

private:
    std::map<std::string, SessionConfig> sessions_;
};

std::unique_ptr<SessionManager> SessionManager::default_platform() {
    return std::make_unique<InMemorySessionManager>();
}

std::string generate_session_id() {
    static uint64_t counter = 0;
    return std::format("dashpay-{:08X}", counter++);
}

} // namespace dashpay
