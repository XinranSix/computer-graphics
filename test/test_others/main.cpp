#include <exception>
#include <iostream>
#include <source_location>
#include <string_view>
#include <utility>

class MyException : public std::exception {
public:
    MyException(std::string message,
                std::source_location location = std::source_location::current())
        : message { std::move(message) }, location { std::move(location) } {
    }

    const char *what() const noexcept override {
        return message.c_str();
    }

    virtual const std::source_location where() const noexcept {
        return location;
    }

private:
    std::string message;
    std::source_location location;
};

struct MyLog {
    bool success { true };
    std::string info {};

    void operator()(std::string_view message,
                    std::source_location const &location =
                        std::source_location::current()) {
        std::cout << message << std::endl;
        std::cout << location.file_name() << std::endl;
        std::cout << location.function_name() << std::endl;
        std::cout << location.line() << std::endl;
        std::cout << location.column() << std::endl;
    }
};

void printInfo() {

    MyLog myLog {};

    myLog("test12312e5mthty");
}

int main() {

    printInfo();

    return 0;
}
