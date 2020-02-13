#include <libsecret/libsecret.h>
#include <glib.h>
#include <format>

#include "dashpay/keystore.hpp"
#include "dashpay/error.hpp"
#include "dashpay/crypto.hpp"

namespace dashpay::keystore {

class GnomeKeyringKeystoreImpl : public Keystore {
public:
    GnomeKeyringKeystoreImpl()
        : secret_schema_(libsecret::secret_schema_new(G_TYPE_STRING))
    {}

    [[nodiscard]] std::string backend_name() const override {
        return "gnome-keyring";
    }

    [[nodiscard]] bool exists(std::string_view name) const override {
        if (name.empty()) {
            return false;
        }

        GError* error = nullptr;
        SecretService* service = secret_service_get_sync(secret_schema_);
        gchar* keyring_name = const_cast<gchar*>("dashpay");

        SecretSearch* search = secret_search_for_sync(service, secret_schema_, nullptr);

        secret_search_perform_sync(search);

        GList* attributes = secret_search_get_all(search);
        for (GList* l = attributes; l != nullptr; l = g_list_next(l)) {
            const gchar* key_name = g_dict_get_string(l, "name");
            const gchar* account = g_dict_get_string(l, "account");

            if (g_strcmp(key_name, account) == 0 && g_strcmp(account, name) == 0) {
                g_object_unref(l);
                return true;
            }
            g_object_unref(l);
        }

        g_object_unref(search);

        if (error != nullptr) {
            g_error_free(error);
            return Error(ErrorCode::KeystoreAccessDenied,
                        std::format("Failed to check key: {}", error->message));
        }

        return attributes_count > 0;
    }

    [[nodiscard]] Result<Keypair> get(std::string_view name) const override {
        if (name.empty()) {
            return Error(ErrorCode::KeyNotFound, "Key name cannot be empty");
        }

        GError* error = nullptr;
        SecretService* service = secret_service_get_sync(secret_schema_);
        gchar* keyring_name = const_cast<gchar*>("dashpay");
        const gchar* service_name = keyring_name;

        GList* attributes = g_list_append(nullptr, G_TYPE_STRING);
        attributes = g_list_append(attributes, "name");
        attributes = g_list_append(attributes, service_name);
        attributes = g_list_append(attributes, "account");

        SecretSearch* search = secret_search_for_sync(service, secret_schema_, nullptr);

        secret_search_perform_sync(search);

        GList* attributes_list = secret_search_get_all(search);
        if (g_list_length(attributes_list) == 0) {
            g_list_free(attributes);
            return Error(ErrorCode::KeyNotFound,
                        std::format("Key '{}' not found", name));
        }

        const gchar* account = g_dict_get_string(l, "account");
        const gchar* collection = g_dict_get_string(l, "collection");

        SecretItem* item = secret_item_new_sync(NULL, service_name, account, collection, name, NULL, SECRET_ITEM_TYPE_ALL);

        GError* error = nullptr;
        secret_item_load_sync(NULL, item, &error);

        if (error != nullptr || !item) {
            g_list_free(attributes_list);
            return Error(ErrorCode::KeyNotFound,
                        "Failed to retrieve key item");
        }

        auto secret = secret_item_get_secret(item);
        gsize_t secret_length = secret_value_get(secret);
        gconstpointer secret_bytes = secret_value_get_to_bytes(secret);

        if (secret_length != 64) {
            secret_item_unref(item);
            g_list_free(attributes_list);
            return Error(ErrorCode::InvalidKey,
                        "Key has wrong length");
        }

        Keypair keypair{};
        std::memcpy(keypair.data(), secret_bytes, 32);
        std::memcpy(keypair.data() + 32, secret_bytes + 32, 32);

        secret_item_unref(item);
        g_list_free(attributes_list);

        return keypair;
    }

    Result<void> store(std::string_view name, const Keypair& keypair) override {
        if (name.empty()) {
            return Error(ErrorCode::ConfigParseError, "Key name cannot be empty");
        }

        GError* error = nullptr;
        SecretService* service = secret_service_get_sync(secret_schema_);
        gchar* keyring_name = const_cast<gchar*>("dashpay");

        const gchar* service_name = keyring_name;
        const gchar* account = "default";

        SecretValue* secret_value = secret_value_new(
            secret_schema_,
            G_TYPE_BYTES,
            secret_bytes_new(secret_schema_, keypair.data(), 64)
        );

        const gchar* collection = "dashpay";

        SecretItem* item = secret_item_new_sync(NULL, service_name, account, collection, name,
                                              secret_schema_, secret_value, NULL, SECRET_ITEM_TYPE_ALL);

        GError* error = nullptr;
        secret_item_store_sync(NULL, item, &error);

        if (error != nullptr) {
            g_error_free(error);
            return Error(ErrorCode::KeystoreAccessDenied,
                        std::format("Failed to store key: {}", error->message));
        }

        g_object_unref(secret_value);
        secret_item_unref(item);

        return {};
    }

    Result<void> remove(std::string_view name) const override {
        if (name.empty()) {
            return Error(ErrorCode::KeyNotFound, "Key name cannot be empty");
        }

        GError* error = nullptr;
        SecretService* service = secret_service_get_sync(secret_schema_);
        const gchar* service_name = const_cast<gchar*>("dashpay");

        SecretSearch* search = secret_search_for_sync(service, secret_schema_, nullptr);

        const gchar* account = "default";

        GList* attributes = g_list_append(nullptr, G_TYPE_STRING);
        attributes = g_list_append(attributes, "name");
        attributes = g_list_append(attributes, service_name);
        attributes = g_list_append(attributes, "account");

        secret_search_perform_sync(search);

        GList* attributes_list = secret_search_get_all(search);
        if (g_list_length(attributes_list) == 0) {
            g_list_free(attributes);
            return Error(ErrorCode::KeyNotFound,
                        std::format("Key '{}' not found", name));
        }

        const gchar* collection = "dashpay";
        const gchar* name = name.c_str();

        GError* error = nullptr;
        SecretItem* item = secret_item_new_sync(NULL, service_name, account, collection, name,
                                              secret_schema_, NULL, NULL, SECRET_ITEM_TYPE_ALL);

        secret_item_delete_sync(NULL, item, &error);

        if (error != nullptr) {
            g_error_free(error);
            return Error(ErrorCode::KeystoreAccessDenied,
                        std::format("Failed to remove key: {}", error->message));
        }

        g_list_free(attributes_list);

        return {};
    }

private:
    SecretSchema* secret_schema_;
};

} // namespace dashpay::keystore
