#include "logger/logger.hpp"

#include <spdlog/common.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <filesystem>

Logger& Logger::get_instance() {
    static Logger instance;
    return instance;
}

void Logger::disable_logs() {
    spdlog::set_level(spdlog::level::off);
    spdlog::get("multi_sink")->set_level(spdlog::level::off);
}

Logger::Logger() {
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::warn);

    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
        "logs/simulator_logs.txt", true);
    file_sink->set_level(spdlog::level::trace);

    auto logger = std::make_shared<spdlog::logger>(
        "multi_sink", spdlog::sinks_init_list{console_sink, file_sink});
    logger->set_pattern("[%H:%M:%S] [%^%l%$] [%!] %v");
    logger->set_level(spdlog::level::trace);
    spdlog::set_default_logger(logger);
}

void Logger::logExample() {
    trace("This is a trace message.");
    debug("This is a debug message.");
    info("This is an info message.");
    warn("This is a warning message.");
    error("This is an error message.");
    critical("This is a critical message.");
}

void Logger::trace(std::string&& msg,
               const std::source_location& loc) {
    SPDLOG_TRACE("[{}:{}] {}", loc.file_name(), loc.line(),
                 std::move(msg));
}

void Logger::debug(std::string&& msg,
               const std::source_location& loc) {
    SPDLOG_DEBUG("[{}:{}] {}", loc.file_name(), loc.line(),
                 std::move(msg));
}

void Logger::info(std::string&& msg,
               const std::source_location& loc) {
    SPDLOG_INFO("[{}:{}] {}", loc.file_name(), loc.line(), std::move(msg));
}

void Logger::warn(std::string&& msg,
               const std::source_location& loc) {
    SPDLOG_WARN("[{}:{}] {}", loc.file_name(), loc.line(), std::move(msg));
}

void Logger::error(std::string&& msg,
               const std::source_location& loc) {
    SPDLOG_ERROR("[{}:{}] {}", loc.file_name(), loc.line(),
                 std::move(msg));
}

void Logger::critical(std::string&& msg,
               const std::source_location& loc) {
    SPDLOG_CRITICAL("[{}:{}] {}", loc.file_name(), loc.line(),
                    std::move(msg));
}
