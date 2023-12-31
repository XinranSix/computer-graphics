#include "log/log.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace ht {

    void Log::init() {

        consoleLogger = spdlog::stdout_color_mt("Console");
        consoleLogger->set_level(spdlog::level::trace);

        fileLogger = spdlog::basic_logger_mt("File", "ht.log");
        fileLogger->set_level(spdlog::level::trace);
    }

    std::shared_ptr<spdlog::logger> Log::consoleLogger;
    std::shared_ptr<spdlog::logger> Log::fileLogger;
} // namespace ht
