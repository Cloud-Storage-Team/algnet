#pragma once

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#define LOG_TRACE(msg) SPDLOG_TRACE(msg)
#define LOG_DEBUG(msg) SPDLOG_DEBUG(msg)
#define LOG_INFO(msg) SPDLOG_INFO(msg)
#define LOG_WARN(msg) SPDLOG_WARN(msg)
#define LOG_ERROR(msg) SPDLOG_ERROR(msg)
#define LOG_CRITICAL(msg) SPDLOG_CRITICAL(msg)

class Logger {
public:
    static void logExample();
    static void setupLogging();
private:
    static std::shared_ptr<spdlog::logger> logger;
};
