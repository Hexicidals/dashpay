#pragma once

#include <optional>
#include <string>
#include <vector>

#include "dashpay/error.hpp"

namespace dashpay {

struct TransactionRecord {
    std::string id;
    std::string from;
    std::string to;
    uint64_t amount;
    uint64_t timestamp;
    std::string signature;
};

class Persistence {
public:
    virtual ~Persistence() = default;

    [[nodiscard]] virtual Result<std::vector<TransactionRecord>> get_history(
        std::string_view pubkey) = 0;

    virtual Result<void> save_transaction(
        const TransactionRecord& record) = 0;

    [[nodiscard]] virtual Result<TransactionRecord*> get_transaction(
        std::string_view id) = 0;
};

class FilePersistence : public Persistence {
public:
    explicit FilePersistence(std::string_view data_dir);

    [[nodiscard]] Result<std::vector<TransactionRecord>> get_history(
        std::string_view pubkey) override;

    Result<void> save_transaction(const TransactionRecord& record) override;

    [[nodiscard]] Result<TransactionRecord*> get_transaction(std::string_view id) override;

private:
    std::string data_dir_;
};

} // namespace dashpay
