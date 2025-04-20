#include "logger.hpp"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

std::shared_ptr<spdlog::logger> Logger::logger;

void Logger::setupLogging() {
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::warn);

    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
        "logs/simulator_logs.txt", true);
    file_sink->set_level(spdlog::level::trace);

    logger = std::make_shared<spdlog::logger>(
        "multi_sink", spdlog::sinks_init_list{console_sink, file_sink});
    logger->set_pattern("[%H:%M:%S] [%^%l%$] [%s:%#] [%!] %v");
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

void Logger::TRACE(std::string msg) { SPDLOG_TRACE(msg); }

void Logger::DEBUG(std::string msg) { SPDLOG_DEBUG(msg); }

void Logger::INFO(std::string msg) { SPDLOG_INFO(msg); }

void Logger::WARN(std::string msg) { SPDLOG_WARN(msg); }

void Logger::ERROR(std::string msg) { SPDLOG_ERROR(msg); }

void Logger::CRITICAL(std::string msg) { SPDLOG_CRITICAL(msg); }
