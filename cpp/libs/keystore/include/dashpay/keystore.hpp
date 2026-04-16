#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "dashpay/crypto.hpp"
#include "dashpay/error.hpp"

namespace dashpay {

enum class KeystoreBackend : uint8_t {
    AppleKeychain,
    WindowsHello,
    GnomeKeyring,
    OnePassword,
    File,
};

[[nodiscard]] std::string_view to_string(KeystoreBackend backend) noexcept;

class Keystore {
public:
    virtual ~Keystore() = default;

    [[nodiscard]] virtual Result<std::string> backend_name() const = 0;

    [[nodiscard]] virtual bool exists(std::string_view name) const = 0;

    [[nodiscard]] virtual Result<Keypair> get(std::string_view name) = 0;

    virtual Result<void> store(
        std::string_view name,
        const Keypair& keypair
    ) = 0;

    virtual Result<void> remove(std::string_view name) = 0;

    [[nodiscard]] static std::unique_ptr<Keystore> default_platform();

    [[nodiscard]] static std::unique_ptr<Keystore> from_string(std::string_view backend);
};

class AppleKeychainKeystore : public Keystore {
public:
    AppleKeychainKeystore();
    [[nodiscard]] std::string backend_name() const override { return "apple-keychain"; }

    [[nodiscard]] bool exists(std::string_view name) const override;
    [[nodiscard]] Result<Keypair> get(std::string_view name) const override;
    Result<void> store(std::string_view name, const Keypair& keypair) override;
    Result<void> remove(std::string_view name) override;
};

class WindowsHelloKeystore : public Keystore {
public:
    WindowsHelloKeystore();
    [[nodiscard]] std::string backend_name() const override { return "windows-hello"; }

    [[nodiscard]] bool exists(std::string_view name) const override;
    [[nodiscard]] Result<Keypair> get(std::string_view name) const override;
    Result<void> store(std::string_view name, const Keypair& keypair) override;
    Result<void> remove(std::string_view name) override;
};

class GnomeKeyringKeystore : public Keystore {
public:
    GnomeKeyringKeystore();
    [[nodiscard]] std::string backend_name() const override { return "gnome-keyring"; }

    [[nodiscard]] bool exists(std::string_view name) const override;
    [[nodiscard]] Result<Keypair> get(std::string_view name) const override;
    Result<void> store(std::string_view name, const Keypair& keypair) override;
    Result<void> remove(std::string_view name) override;
};

class OnePasswordKeystore : public Keystore {
public:
    explicit OnePasswordKeystore(std::string_view vault);
    explicit OnePasswordKeystore(std::string_view vault, std::string_view account);

    [[nodiscard]] std::string backend_name() const override { return "1password"; }

    [[nodiscard]] bool exists(std::string_view name) const override;
    [[nodiscard]] Result<Keypair> get(std::string_view name) const override;
    Result<void> store(std::string_view name, const Keypair& keypair) override;
    Result<void> remove(std::string_view name) override;
};

class FileKeystore : public Keystore {
public:
    explicit FileKeystore(std::string_view base_dir);

    [[nodiscard]] std::string backend_name() const override { return "file"; }

    [[nodiscard]] bool exists(std::string_view name) const override;
    [[nodiscard]] Result<Keypair> get(std::string_view name) const override;
    Result<void> store(std::string_view name, const Keypair& keypair) override;
    Result<void> remove(std::string_view name) override;

private:
    std::string base_dir_;
    [[nodiscard]] std::string key_path(std::string_view name) const;
};

} // namespace dashpay
