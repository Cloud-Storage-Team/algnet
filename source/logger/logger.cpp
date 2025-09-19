#include "logger/logger.hpp"

#include <spdlog/common.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <filesystem>

std::string Logger::m_output_dir = "";
bool Logger::m_is_initialized = false;

Logger& Logger::get_instance() {
    static Logger instance;
    return instance;
}

void Logger::set_output_dir(std::string dir) {
    if (m_is_initialized) {
        LOG_ERROR(fmt::format(
            "Set logger output dir {} when LOgger already initialized "
            "with output_dir {}; no effect",
            dir, m_output_dir));
        return;
    }
    m_output_dir = dir;
}

void Logger::disable_logs() {
    spdlog::set_level(spdlog::level::off);
    spdlog::get("multi_sink")->set_level(spdlog::level::off);
}

Logger::Logger() {
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::warn);

    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
        std::filesystem::path(m_output_dir) / "nons_logs.txt", true);
    file_sink->set_level(spdlog::level::trace);

    auto logger = std::make_shared<spdlog::logger>(
        "multi_sink", spdlog::sinks_init_list{console_sink, file_sink});
    logger->set_pattern("[%H:%M:%S] [%^%l%$] [%!] %v");
    logger->set_level(spdlog::level::trace);
    spdlog::set_default_logger(logger);
    m_is_initialized = true;
}

void Logger::logExample() {
    trace("This is a trace message.");
    debug("This is a debug message.");
    info("This is an info message.");
    warn("This is a warning message.");
    error("This is an error message.");
    critical("This is a critical message.");
}

static constexpr std::string_view file_name_from_path(std::string_view path) {
    std::size_t pos = path.find_last_of("/\\");
    if (pos == std::string_view::npos)
        return path;
    else
        return path.substr(pos + 1);
}

void Logger::trace(std::string&& msg, const std::source_location& loc) {
    SPDLOG_TRACE("[{}:{}] {}", file_name_from_path(loc.file_name()), loc.line(),
                 std::move(msg));
}

void Logger::debug(std::string&& msg, const std::source_location& loc) {
    SPDLOG_DEBUG("[{}:{}] {}", file_name_from_path(loc.file_name()), loc.line(),
                 std::move(msg));
}

void Logger::info(std::string&& msg, const std::source_location& loc) {
    SPDLOG_INFO("[{}:{}] {}", file_name_from_path(loc.file_name()), loc.line(),
                std::move(msg));
}

void Logger::warn(std::string&& msg, const std::source_location& loc) {
    SPDLOG_WARN("[{}:{}] {}", file_name_from_path(loc.file_name()), loc.line(),
                std::move(msg));
}

void Logger::error(std::string&& msg, const std::source_location& loc) {
    SPDLOG_ERROR("[{}:{}] {}", file_name_from_path(loc.file_name()), loc.line(),
                 std::move(msg));
}

void Logger::critical(std::string&& msg, const std::source_location& loc) {
    SPDLOG_CRITICAL("[{}:{}] {}", file_name_from_path(loc.file_name()),
                    loc.line(), std::move(msg));
}
