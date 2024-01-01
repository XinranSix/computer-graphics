#pragma once

#include <memory>
#include <spdlog/spdlog.h>
#include <string_view>

namespace ht {
    class Log {
    public:
        static void init(std::string_view file = "./ht.log");

        inline static std::shared_ptr<spdlog::logger> &getConsoleLogger() {
            return consoleLogger;
        }

        inline static std::shared_ptr<spdlog::logger> &getFileLogger() {
            return fileLogger;
        }

    private:
        static std::shared_ptr<spdlog::logger> consoleLogger;
        static std::shared_ptr<spdlog::logger> fileLogger;
    };
} // namespace ht

#define HT_CONSOLE_TRACE(...) ::ht::Log::getConsoleLogger()->trace(__VA_ARGS__)
#define HT_CONSOLE_DEBUG(...) ::ht::Log::getConsoleLogger()->debug(__VA_ARGS__)
#define HT_CONSOLE_INFO(...) ::ht::Log::getConsoleLogger()->info(__VA_ARGS__)
#define HT_CONSOLE_WARN(...) ::ht::Log::getConsoleLogger()->warn(__VA_ARGS__)
#define HT_CONSOLE_ERROE(...) ::ht::Log::getConsoleLogger()->error(__VA_ARGS__)
#define HT_CONSOLE_CRITICAL(...)                                               \
    ::ht::Log::getConsoleLogger()->critical(__VA_ARGS__)

#define HT_FILE_TRACE(...) ::ht::Log::getFileLogger()->trace(__VA_ARGS__)
#define HT_FILE_DEBUG(...) ::ht::Log::getFileLogger()->debug(__VA_ARGS__)
#define HT_FILE_INFO(...) ::ht::Log::getFileLogger()->info(__VA_ARGS__)
#define HT_FILE_WARN(...) ::ht::Log::getFileLogger()->warn(__VA_ARGS__)
#define HT_FILE_ERROE(...) ::ht::Log::getFileLogger()->error(__VA_ARGS__)
#define HT_FILE_CRITICAL(...) ::ht::Log::getFileLogger()->critical(__VA_ARGS__)
