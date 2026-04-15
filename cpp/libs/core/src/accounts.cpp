// Account management implementation

#include <dashpay/accounts.hpp>
#include <dashpay/config.hpp>
#include <dashpay/crypto.hpp>
#include <dashpay/error.hpp>
#include <dashpay/keystore.hpp>

namespace dashpay::accounts {

class AccountsManagerImpl : public AccountsManager {
private:
    std::shared_ptr<Keystore> keystore_;
    std::shared_ptr<Config> config_;

public:
    AccountsManagerImpl()
        : keystore_(Keystore::default_platform()),
          config_(Config::load()) {}

    [[nodiscard]] Result<std::vector<Account>> list_accounts() const override {
        std::vector<Account> accounts;

        // For now, just return default account
        auto keypair = keystore_->get("default");
        if (!keypair) {
            // Generate keypair if doesn't exist
            auto result = Crypto::generate_keypair();
            if (!result) {
                return std::unexpected(result.error());
            }

            keypair = *result;
            auto store_result = keystore_->store("default", *keypair);
            if (!store_result) {
                return std::unexpected(store_result.error());
            }
        }

        // Create account from keypair
        std::string pubkey = Crypto::encode_base58(
            std::span<const std::uint8_t>(keypair->data() + 32, 32)
        );

        Account account{
            .name = "default",
            .public_key = pubkey,
            .created_at = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()
            ).count(),
            .balance = 0,
            .network = Network::Localnet,
        };

        accounts.push_back(account);

        return accounts;
    }

    [[nodiscard]] Result<Account> get_account(std::string_view name) const override {
        auto keypair = keystore_->get(std::string(name));
        if (!keypair) {
            return Error(ErrorCode::KeyNotFound,
                        std::format("Account not found: {}", name));
        }

        std::string pubkey = Crypto::encode_base58(
            std::span<const std::uint8_t>(keypair->data() + 32, 32)
        );

        Account account{
            .name = std::string(name),
            .public_key = pubkey,
            .created_at = 0, // Unknown
            .balance = 0,
            .network = Network::Localnet,
        };

        return account;
    }

    Result<void> create_account(
        std::string_view name,
        Network network
    ) override {
        auto keypair = Crypto::generate_keypair();
        if (!keypair) {
            return std::unexpected(keypair.error());
        }

        auto store_result = keystore_->store(name, *keypair);
        if (!store_result) {
            return std::unexpected(store_result.error());
        }

        return {};
    }

    [[nodiscard]] Result<std::string> get_balance(std::string_view pubkey) const override {
        auto config = config_ ? config_ : Config::load();
        if (!config) {
            return std::format("Balance: {} (local)", pubkey);
        }

        // In production, would query RPC
        return std::format("Balance: 0.00 DASH for {}", pubkey);
    }

    [[nodiscard]] std::string backend_name() const override {
        return keystore_->backend_name();
    }

private:
    void update_account_balance(
        std::string_view name,
        uint64_t balance
    ) {
        // In production, would update local state
        // For now, just log
        std::cout << "Updated balance for account " << name << ": " << balance << std::endl;
    }

    std::string encode_balance(std::string_view pubkey, uint64_t balance) {
        // Format as DASH with 8 decimal places
        double dash = static_cast<double>(balance) / 100000000.0;

        return std::format("{:.8f} DASH", dash);
    }
};

std::unique_ptr<AccountsManager> AccountsManager::default_platform() {
    return std::make_unique<AccountsManagerImpl>();
}

} // namespace dashpay::accounts
