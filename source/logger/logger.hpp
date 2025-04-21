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

    void TRACE(std::string msg, std::source_location loc =
                                           std::source_location::current());
    void DEBUG(std::string msg, std::source_location loc =
                                           std::source_location::current());
    void INFO(std::string msg, std::source_location loc =
                                          std::source_location::current());
    void WARN(std::string msg, std::source_location loc =
                                          std::source_location::current());
    void ERROR(std::string msg, std::source_location loc =
                                           std::source_location::current());
    void CRITICAL(std::string msg, std::source_location loc =
                                              std::source_location::current());

private:
    Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
};

#define LOG_TRACE(...) Logger::get_instance().TRACE(__VA_ARGS__)
#define LOG_DEBUG(...) Logger::get_instance().DEBUG(__VA_ARGS__)
#define LOG_INFO(...) Logger::get_instance().INFO(__VA_ARGS__)
#define LOG_WARN(...) Logger::get_instance().WARN(__VA_ARGS__)
#define LOG_ERROR(...) Logger::get_instance().ERROR(__VA_ARGS__)
#define LOG_CRITICAL(...) Logger::get_instance().CRITICAL(__VA_ARGS__)
