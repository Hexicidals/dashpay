#include <dashpay/error.hpp>

namespace dashpay {

std::string Error::to_string() const {
    if (message_.empty()) {
        return std::format("Error: {}", to_string(code_));
    }
    return std::format("Error: {} - {}", to_string(code_), message_);
}

} // namespace dashpay
