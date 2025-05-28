#pragma once

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include <memory>
#include <source_location>

namespace spdlog {
class logger;
}  // namespace spdlog

class Logger {
public:
    static Logger& get_instance();

    void logExample();
    void disable_logs();

    void trace(std::string msg,
               std::source_location loc = std::source_location::current());
    void debug(std::string msg,
               std::source_location loc = std::source_location::current());
    void info(std::string msg,
              std::source_location loc = std::source_location::current());
    void warn(std::string msg,
              std::source_location loc = std::source_location::current());
    void error(std::string msg,
               std::source_location loc = std::source_location::current());
    void critical(std::string msg,
                  std::source_location loc = std::source_location::current());

private:
    Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
};

#define LOG_TRACE(...) Logger::get_instance().trace(__VA_ARGS__)
#define LOG_DEBUG(...) Logger::get_instance().debug(__VA_ARGS__)
#define LOG_INFO(...) Logger::get_instance().info(__VA_ARGS__)
#define LOG_WARN(...) Logger::get_instance().warn(__VA_ARGS__)
#define LOG_ERROR(...) Logger::get_instance().error(__VA_ARGS__)
#define LOG_CRITICAL(...) Logger::get_instance().critical(__VA_ARGS__)
