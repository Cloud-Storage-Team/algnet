#pragma once

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include <source_location>
#include <string>

namespace spdlog {
class logger;
}  // namespace spdlog

class Logger {
public:
    static Logger& get_instance();

    void logExample();
    void disable_logs();

    void trace(std::string&& msg, const std::source_location& loc =
                                      std::source_location::current());
    void debug(std::string&& msg,
               const std::source_location& = std::source_location::current());
    void info(std::string&& msg,
              const std::source_location& = std::source_location::current());
    void warn(std::string&& msg,
              const std::source_location& = std::source_location::current());
    void error(std::string&& msg,
               const std::source_location& = std::source_location::current());
    void critical(std::string&& msg, const std::source_location& =
                                         std::source_location::current());

private:
    Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
};

#define LOG_LEVEL_TRACE 0
#define LOG_LEVEL_DEBUG 1
#define LOG_LEVEL_INFO 2
#define LOG_LEVEL_WARN 3
#define LOG_LEVEL_ERROR 4
#define LOG_LEVEL_CRIT 5

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_TRACE
#endif

#if LOG_LEVEL <= LOG_LEVEL_TRACE
#define LOG_TRACE(...) Logger::get_instance().trace(__VA_ARGS__)
#else
#define LOG_TRACE(...)
#endif

#if LOG_LEVEL <= LOG_LEVEL_DEBUG
#define LOG_DEBUG(...) Logger::get_instance().debug(__VA_ARGS__)
#else
#define LOG_DEBUG(...)
#endif

#if LOG_LEVEL <= LOG_LEVEL_INFO
#define LOG_INFO(...) Logger::get_instance().info(__VA_ARGS__)
#else
#define LOG_INFO(...)
#endif

#if LOG_LEVEL <= LOG_LEVEL_WARN
#define LOG_WARN(...) Logger::get_instance().warn(__VA_ARGS__)
#else
#define LOG_WARN(...)
#endif

#if LOG_LEVEL <= LOG_LEVEL_ERROR
#define LOG_ERROR(...) Logger::get_instance().error(__VA_ARGS__)
#else
#define LOG_ERROR(...)
#endif

#if LOG_LEVEL <= LOG_LEVEL_CRIT
#define LOG_CRITICAL(...) Logger::get_instance().critical(__VA_ARGS__)
#else
#define LOG_CRITICAL(...)
#endif