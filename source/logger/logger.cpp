#include "logger.hpp"

std::shared_ptr<spdlog::logger> Logger::logger;

void Logger::setupLogging() {
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::warn);

    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/simulator_logs.txt", true);
    file_sink->set_level(spdlog::level::trace);

    logger = std::make_shared<spdlog::logger>("multi_sink", spdlog::sinks_init_list{console_sink, file_sink});
    logger->set_pattern("[%H:%M:%S] [%^%l%$] [%s:%#] [%!] %v");
    logger->set_level(spdlog::level::trace);
    spdlog::set_default_logger(logger);
}

void Logger::logExample() {
    LOG_TRACE("This is a trace message. Error: {}", "Error description");
    LOG_TRACE("This is a trace message.");
    LOG_DEBUG("This is a debug message.");
    LOG_INFO("This is an info message.");
    LOG_WARN("This is a warning message. Error: {}", "Error description");
    LOG_ERROR("This is an error message.");
    LOG_CRITICAL("This is a critical message.");
}
