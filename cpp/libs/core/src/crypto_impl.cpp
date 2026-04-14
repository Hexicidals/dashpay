#include <dashpay/crypto.hpp>

namespace dashpay {

std::unique_ptr<Crypto> Crypto::default_platform() {
#if defined(__APPLE__)
    return std::make_unique<CryptoImplApple>();
#elif defined(_WIN32)
    return std::make_unique<CryptoImplWindows>();
#elif defined(__linux__)
    return std::make_unique<CryptoImplLinux>();
#else
    return std::make_unique<CryptoImplGeneric>();
#endif
}

std::unique_ptr<Crypto> create_crypto() {
#if defined(__APPLE__)
    return std::make_unique<CryptoImplApple>();
#elif defined(_WIN32)
    return std::make_unique<CryptoImplWindows>();
#elif defined(__linux__)
    return std::make_unique<CryptoImplLinux>();
#else
    return std::make_unique<CryptoImplGeneric>();
#endif
}

std::string Crypto::backend_name() const {
    return default_platform() ? default_platform()->backend_name() : "generic";
}

} // namespace dashpay
