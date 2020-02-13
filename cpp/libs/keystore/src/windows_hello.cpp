#include <windows.h>
#include <wincred.h>
#include <tchar.h>
#include <format>

#include "dashpay/keystore.hpp"
#include "dashpay/crypto.hpp"
#include "dashpay/error.hpp"

namespace dashpay::keystore {

class WindowsHelloKeystoreImpl : public Keystore {
public:
    WindowsHelloKeystoreImpl()
        : target_name_(L"dashpay")
    {}

    [[nodiscard]] std::string backend_name() const override {
        return "windows-hello";
    }

    [[nodiscard]] bool exists(std::string_view name) const override {
        CREDENTIALW target_name;
        CREDENTIAL credential{};

        auto success = CredReadW(target_name_, name, &credential, 1);
        CredFree(&credential);

        return success == TRUE;
    }

    [[nodiscard]] Result<Keypair> get(std::string_view name) const override {
        CREDENTIALW target_name;
        CREDENTIAL credential{};

        // Get credential - this reads both username and password
        auto success = CredReadW(target_name_, name, &credential, 0);

        if (!success) {
            return Error(ErrorCode::KeyNotFound,
                        "Key not found in Windows Credential Manager");
        }

        DWORD len = 0;
        success = CredGetAttribute(target_name_, &credential, NULL, CRED_ATTRIBUTE_CREDENTIAL_BLOB, &len);

        if (success && len == 64) {
            Keypair keypair{};
            std::memcpy(keypair.data(), credential.CredentialBlob, 32);
            std::memcpy(keypair.data() + 32, reinterpret_cast<const uint8_t*>(credential.CredentialBlob) + 32, 32);

            CredFree(&credential);
            return keypair;
        }

        CredFree(&credential);

        return Error(ErrorCode::KeyNotFound,
                    "Stored key has invalid format or length");
    }

    Result<void> store(std::string_view name, const Keypair& keypair) override {
        CREDENTIALW target_name;
        CREDENTIAL credential{};

        // Store as credential blob
        credential.CredentialBlob = keypair.data();
        credential.CredentialBlobSize = 64;
        credential.Type = CRED_TYPE_GENERIC;
        credential.Persist = 1;

        // Set username to "key" for consistency
        size_t len = lstrlenW(L"key");
        credential.UserName = (LPWSTR)malloc((len + 1) * sizeof(WCHAR));
        std::memcpy(credential.UserName, L"key", (len + 1) * sizeof(WCHAR));

        auto success = CredWriteW(target_name_, name, &credential, CRED_PERSIST_LOCAL_MACHINE);
        CredFree(&credential);

        if (!success) {
            return Error(ErrorCode::KeystoreAccessDenied,
                        "Failed to store key in Windows Credential Manager");
        }

        CredFree(credential.UserName);
        CredFree(&credential);

        return {};
    }

    Result<void> remove(std::string_view name) const override {
        CREDENTIALW target_name;

        auto success = CredDeleteW(target_name_, name);
        if (success != ERROR_SUCCESS) {
            auto error_code = GetLastError();
            return Error(ErrorCode::KeystoreAccessDenied,
                        std::format("Failed to delete key: Error code {}", error_code));
        }

        return {};
    }

private:
    LPCWSTR target_name_;
};

std::unique_ptr<Keystore> WindowsHelloKeystore::default_platform() {
    return std::make_unique<WindowsHelloKeystoreImpl>();
}

} // namespace dashpay::keystore
