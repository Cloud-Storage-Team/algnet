#pragma once

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include <memory>
#include <source_location>

namespace spdlog {
class logger;
}  // namespace spdlog

class Logger {
public:
    static void logExample();
    static void setupLogging();

    static void TRACE(std::string msg, std::source_location loc =
                                           std::source_location::current());
    static void DEBUG(std::string msg, std::source_location loc =
                                           std::source_location::current());
    static void INFO(std::string msg, std::source_location loc =
                                          std::source_location::current());
    static void WARN(std::string msg, std::source_location loc =
                                          std::source_location::current());
    static void ERROR(std::string msg, std::source_location loc =
                                           std::source_location::current());
    static void CRITICAL(std::string msg, std::source_location loc =
                                              std::source_location::current());

private:
    static std::shared_ptr<spdlog::logger> logger;
};
