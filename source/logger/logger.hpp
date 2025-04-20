#pragma once

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include <memory>

namespace spdlog {
    class logger;
} // namespace spdlog

class Logger {
public:
    static void logExample();
    static void setupLogging();

    static void TRACE(std::string msg);
    static void DEBUG(std::string msg);
    static void INFO(std::string msg);
    static void WARN(std::string msg);
    static void ERROR(std::string msg);
    static void CRITICAL(std::string msg);

private:
    static std::shared_ptr<spdlog::logger> logger;
};
