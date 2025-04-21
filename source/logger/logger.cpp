#include "logger.hpp"

#include <spdlog/common.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <memory>
#include <source_location>
#include <filesystem>

Logger& Logger::get_instance() {
    static Logger instance;
    return instance;
}

Logger::Logger() {
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::warn);

    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
        "logs/simulator_logs.txt", true);
    file_sink->set_level(spdlog::level::trace);

    logger = std::make_unique<spdlog::logger>(
        "multi_sink", spdlog::sinks_init_list{console_sink, file_sink});
    logger->set_pattern("[%H:%M:%S] [%^%l%$] [%!] %v");
    logger->set_level(spdlog::level::trace);
    spdlog::set_default_logger(logger);   
}

void Logger::logExample() {
    TRACE("This is a trace message.");
    DEBUG("This is a debug message.");
    INFO("This is an info message.");
    WARN("This is a warning message.");
    ERROR("This is an error message.");
    CRITICAL("This is a critical message.");
}

static std::string parse_filename(std::string_view str) {
    return std::filesystem::path(str).filename().string();
}

void Logger::TRACE(std::string msg, std::source_location loc) {
    SPDLOG_TRACE("[{}:{}] {}", parse_filename(loc.file_name()), loc.line(), msg);
}

void Logger::DEBUG(std::string msg, std::source_location loc) {
    SPDLOG_DEBUG("[{}:{}] {}", parse_filename(loc.file_name()), loc.line(), msg);
}

void Logger::INFO(std::string msg, std::source_location loc) {
    SPDLOG_INFO("[{}:{}] {}", parse_filename(loc.file_name()), loc.line(), msg);
}

void Logger::WARN(std::string msg, std::source_location loc) {
    SPDLOG_WARN("[{}:{}] {}", parse_filename(loc.file_name()), loc.line(), msg);
}

void Logger::ERROR(std::string msg, std::source_location loc) {
    SPDLOG_ERROR("[{}:{}] {}", parse_filename(loc.file_name()), loc.line(), msg);
}

void Logger::CRITICAL(std::string msg, std::source_location loc) {
    SPDLOG_CRITICAL("[{}:{}] {}", parse_filename(loc.file_name()), loc.line(), msg);
}
