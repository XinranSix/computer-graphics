#pragma once

#include <exception>
#include <source_location>
#include <string>

namespace ht {
    class LocationException : public std::exception {
    public:
        LocationException(
            std::string message,
            std::source_location location = std::source_location::current());

        const char *what() const noexcept override;

        virtual const std::source_location where() const noexcept;

    private:
        std::string message;
        std::source_location location;
    };

} // namespace ht
