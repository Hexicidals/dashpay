#include <dashpay/keystore.hpp>

#ifdef __APPLE__
#include <Foundation/Foundation.h>
#include <Security/Security.h>
#include <cstring>

namespace dashpay::keystore {

class AppleKeychainKeystoreImpl : public Keystore {
public:
    AppleKeychainKeystoreImpl()
        : service_name_("dashpay")
    {}

    [[nodiscard]] std::string backend_name() const override {
        return "apple-keychain";
    }

    [[nodiscard]] bool exists(std::string_view name) const override {
        auto query = [NSString stringWithFormat:@"service == '%@' AND account == '%@'", service_name_.c_str(), name.c_str()];
        auto status = status_query();
        return [status count] > 0;
    }

    [[nodiscard]] Result<Keypair> get(std::string_view name) const override {
        auto query = [NSString stringWithFormat:@"service == '%@' AND account == '%@'", service_name_.c_str(), name.c_str()];
        CFTypeRef type_ref = kSecClassKeyItemClass;
        auto result = (__bridge_transfer CFTypeRef)SecItemCopyMatching(query, type_ref, kCFReturnDefault);
        if (result) {
            auto key_data = (__bridge_transfer id)SecItemCopyMatchingWithError((__bridge_transfer id)SecItemCopyMatching)(result, type_ref));

            NSData* key_data_raw = [key_data data];
            NSUInteger key_len = [key_data length];

            if (key_len != 64) {
                return Error(ErrorCode::KeyNotFound,
                    "Invalid key data length from keychain");
            }

            Keypair keypair{};
            std::memcpy(keypair.data(), key_data_raw.bytes, 32);
            std::memcpy(keypair.data() + 32, key_data_raw.bytes + 32, 32);

            return keypair;
        }

        auto error_ref = (__bridge_transfer id)SecItemCopyMatchingWithError)(result, type_ref);
        return Error(ErrorCode::KeystoreAccessDenied,
                    std::format("Key not found: {}", [error_ref localizedDescription]));
    }

    Result<void> store(std::string_view name, const Keypair& keypair) override {
        auto query = [NSString stringWithFormat:@"service == '%@' AND account == '%@'", service_name_.c_str(), name.c_str()];
        CFTypeRef type_ref = kSecClassKeyItemClass;
        CFTypeRef value_ref = kSecValueData;

        auto key_data = [NSMutableDictionary dictionaryWithObjectsAndKeys:@{
            (__bridge NSString *)service_name_.c_str(): (__bridge NSString *)name.c_str(),
            (__bridge NSString *)kSecAttrServiceType: kSecAttrServiceType,
            kSecAttrAccountType: kSecAttrAccountTypeGenericPassword,
            kSecAttrAccessibleWhenUnlocked: (__bridge NSString *)kCFBooleanTrue,
            kSecClassKeyItem: kSecAttrKeyClassPrivate,
            kSecAttrKeySizeInBits: (__bridge NSString *)kSecAttrKeySizeInBits,
            kSecAttrKeySizeInBytes: (__bridge NSString *)kSecAttrKeySizeInBytes,
            kSecAttrIsPermanent: (__bridge NSString *)kCFBooleanTrue,
            kSecAttrIsSensitive: (__bridge NSString *)kCFBooleanTrue,
        }];

        NSData* value_data = [NSData dataWithBytes:keypair.data() length:64];

        [key_data setObject:value_data forKey:value_ref];

        auto status_query = [NSDictionary dictionaryWithObjectsAndKeys:@{
            (__bridge NSString *)service_name_.c_str(): (__bridge NSString *)name.c_str(),
            (__bridge NSString *)kSecAttrServiceType: kSecAttrServiceType,
            kSecAttrAccountType: kSecAttrAccountTypeGenericPassword,
            kSecClassKeyItem: kSecAttrKeyClassPrivate,
            kSecMatchLimit: (__bridge NSString *)kSecMatchLimitOne,
            kSecReturnData: (__bridge NSString *)kCFBooleanTrue,
            kSecUseOperationPrompt: (__bridge NSString *)kCFBooleanTrue,
        }];

        auto result = (__bridge_transfer CFTypeRef)SecItemAdd((__bridge_transfer CFDictionaryRef)status_query, type_ref, value_ref);

        if (result != errSecSuccess) {
            auto error_ref = (__bridge_transfer CFTypeRef)SecItemAdd((__bridge_transfer CFTypeRef)status_query, type_ref, value_ref);
            return Error(ErrorCode::KeystoreAccessDenied,
                        std::format("Failed to store key: {}", [error_ref localizedDescription]));
        }

        return {};
    }

    Result<void> remove(std::string_view name) const override {
        auto query = [NSString stringWithFormat:@"service == '%@' AND account == '%@'", service_name_.c_str(), name.c_str()];
        CFTypeRef type_ref = kSecClassKeyItemClass;
        CFTypeRef value_ref = kSecValueData;

        auto status_query = [NSDictionary dictionaryWithObjectsAndKeys:@{
            (__bridge NSString *)service_name_.c_str(): (__bridge NSString *)name.c_str(),
            (__bridge NSString *)kSecAttrServiceType: kSecAttrServiceType,
            kSecAttrAccountType: kSecAttrAccountTypeGenericPassword,
            kSecClassKeyItem: kSecAttrKeyClassPrivate,
        }];

        auto result = (__bridge_transfer CFTypeRef)SecItemDelete((__bridge_transfer CFDictionaryRef)status_query, type_ref, value_ref);

        if (result != errSecSuccess) {
            auto error_ref = (__bridge_transfer CFTypeRef)SecItemDelete((__bridge_transfer CFTypeRef)status_query, type_ref, value_ref);
            return Error(ErrorCode::KeystoreAccessDenied,
                        std::format("Failed to remove key: {}", [error_ref localizedDescription]));
        }

        return {};
    }

private:
    NSString* service_name_;
};

} // namespace dashpay::keystore

#endif // __APPLE__
