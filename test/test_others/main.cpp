#include <iostream>
#include <fstream>
#include <string>
#include <ctime>

enum class LogLevel { INFO, WARNING, ERROR };

class ILogger {
public:
    ILogger(const std::string &logFileName) : logFileName(logFileName) {
        logFile.open(logFileName, std::ios::app);
        if (!logFile.is_open()) {
            std::cerr << "Error opening log file: " << logFileName << std::endl;
        }
    }

    virtual ~ILogger() {
        if (logFile.is_open()) {
            logFile.close();
        }
    }

    virtual void log(LogLevel level, const std::string &message) {
        std::string logLevelStr;

        switch (level) {
        case LogLevel::INFO:
            logLevelStr = "[INFO] ";
            break;
        case LogLevel::WARNING:
            logLevelStr = "[WARNING] ";
            break;
        case LogLevel::ERROR:
            logLevelStr = "[ERROR] ";
            break;
        }

        // 获取当前时间
        std::time_t now = std::time(nullptr);
        char timestamp[64];
        std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S",
                      std::localtime(&now));

        // 构建日志条目
        std::string logEntry =
            "[" + std::string(timestamp) + "] " + logLevelStr + message;

        // 输出到控制台
        std::cout << logEntry << std::endl;

        // 输出到文件
        if (logFile.is_open()) {
            logFile << logEntry << std::endl;
        }
    }

private:
    std::string logFileName;
    std::ofstream logFile;
};

int main() {
    ILogger logger("log.txt");

    // 使用日志
    logger.log(LogLevel::INFO, "This is an informational message.");
    logger.log(LogLevel::WARNING, "This is a warning message.");
    logger.log(LogLevel::ERROR, "This is an error message.");

    return 0;
}
