#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "dashpay/error.hpp"
#include "dashpay/types.hpp"

namespace dashpay {

struct SessionConfig {
    std::string api_key;
    std::string fee_payer_pubkey;
    uint64_t max_lamports_per_request = 10000000000;
    std::vector<std::string> allowed_origins;
    std::vector<std::string> blocked_origins;
};

struct Session {
    std::string id;
    std::string api_key;
    std::optional<std::string> fee_payer;
    uint64_t created_at;
    uint64_t last_used_at;
    uint64_t total_lamports_sent;
    std::vector<std::string> authorized_origins;
};

class SessionManager {
public:
    explicit SessionManager(std::string_view data_dir);

    [[nodiscard]] Result<Session> create(const SessionConfig& config);
    [[nodiscard]] Result<Session> get(std::string_view id) const;
    [[nodiscard]] Result<std::vector<Session>> list() const;

    [[nodiscard]] Result<void> authorize(
        std::string_view session_id,
        std::string_view origin
    );

    [[nodiscard]] Result<void> revoke(
        std::string_view session_id,
        std::string_view origin
    );

    [[nodiscard]] Result<bool> check_quota(
        const Session& session,
        uint64_t requested_amount
    ) const;

    [[nodiscard]] Result<void> record_usage(
        std::string_view session_id,
        uint64_t lamports_used
    );

    Result<void> cleanup_expired(uint64_t ttl_seconds);

private:
    std::string data_dir_;
};

} // namespace dashpay
