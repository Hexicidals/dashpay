#pragma once

#include <chrono>
#include <filesystem>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

#include "dashpay/error.hpp"

namespace dashpay {

inline constexpr std::string_view ACCOUNTS_FILE = "~/.config/dashpay/accounts.yml";
inline constexpr uint32_t ACCOUNTS_SCHEMA_VERSION = 2;
inline constexpr std::string_view DEFAULT_ACCOUNT_NAME = "default";
inline constexpr std::string_view MAINNET_NETWORK = "mainnet";

[[nodiscard]] bool default_auth_required_for_network(std::string_view network) noexcept;

enum class Keystore : uint8_t {
    AppleKeychain,
    GnomeKeyring,
    WindowsHello,
    OnePassword,
    File,
    Ephemeral,
};

[[nodiscard]] std::string_view to_string(Keystore ks) noexcept;

struct Account {
    Keystore keystore;
    bool active = false;
    std::optional<bool> auth_required;
    std::optional<std::string> pubkey;
    std::optional<std::string> vault;
    std::optional<std::string> account;
    std::optional<std::string> path;
    std::optional<std::string> secret_key_b58;
    std::optional<std::string> created_at;

    [[nodiscard]] bool auth_required_for_network(std::string_view network) const noexcept;
    [[nodiscard]] std::optional<std::string> signer_source(std::string_view name) const;
    [[nodiscard]] std::optional<std::vector<std::uint8_t>> ephemeral_keypair_bytes() const;
};

struct AccountsFile {
    uint32_t version = ACCOUNTS_SCHEMA_VERSION;
    std::map<std::string, std::map<std::string, Account>> accounts;

    static Result<AccountsFile> load();
    Result<void> save() const;

    [[nodiscard]] std::optional<std::pair<std::string_view, const Account*>> account_for_network(
        std::string_view network) const;

    [[nodiscard]] const Account* named_account_for_network(
        std::string_view network, std::string_view name) const;

    [[nodiscard]] std::optional<std::pair<std::string_view, const Account*>> default_account() const;

    void upsert(std::string_view network, std::string_view name, Account account);
    std::optional<Account> remove(std::string_view network, std::string_view name);
    void set_active(std::string_view network, std::string_view name);
};

enum class AccountChoice {
    Resolved,
    Missing,
};

struct ResolvedAccount {
    std::string name;
    Account account;
};

[[nodiscard]] AccountChoice resolve_account_for_network(
    std::string_view network, const AccountsFile& file);

class AccountsStore {
public:
    virtual ~AccountsStore() = default;

    [[nodiscard]] virtual Result<AccountsFile> load() = 0;
    virtual Result<void> save(const AccountsFile& file) = 0;
};

class FileAccountsStore : public AccountsStore {
public:
    static FileAccountsStore default_path();

    explicit FileAccountsStore(std::filesystem::path path);

    [[nodiscard]] const std::filesystem::path& path() const noexcept { return path_; }

    [[nodiscard]] Result<AccountsFile> load() override;
    Result<void> save(const AccountsFile& file) override;

private:
    std::filesystem::path path_;
};

class MemoryAccountsStore : public AccountsStore {
public:
    MemoryAccountsStore() = default;

    explicit MemoryAccountsStore(AccountsFile file);

    [[nodiscard]] uint32_t save_count() const noexcept;
    [[nodiscard]] AccountsFile snapshot() const;

    [[nodiscard]] Result<AccountsFile> load() override;
    Result<void> save(const AccountsFile& file) override;

private:
    mutable std::mutex mutex_;
    AccountsFile file_;
    uint32_t save_count_ = 0;
};

struct ResolvedEphemeral {
    std::string network;
    std::string account_name;
    Account account;
    bool created = false;
};

[[nodiscard]] Result<ResolvedEphemeral> load_or_create_ephemeral_for_network(
    std::string_view network, const AccountsStore& store);

[[nodiscard]] Result<ResolvedEphemeral> load_or_create_ephemeral_for_network_as(
    std::string_view network, std::string_view account_name, const AccountsStore& store);

[[nodiscard]] Result<ResolvedEphemeral> load_or_create_exact_ephemeral_for_network_as(
    std::string_view network, std::string_view account_name, const AccountsStore& store);

} // namespace dashpay
