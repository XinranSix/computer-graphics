#include <source_location>

#include "exception/exception.h"

#include <utility>

namespace ht {

    LocationException::LocationException(std::string message, std::source_location location)
        : message { std::move(message) }, location { std::move(location) } {
    }

    const char *LocationException::what() const noexcept {
        return message.c_str();
    }

    const std::source_location LocationException::where() const noexcept {
        return location;
    }

} // namespace ht
