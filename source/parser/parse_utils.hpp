#pragma once
#include <spdlog/fmt/fmt.h>
#include <yaml-cpp/yaml.h>

#include <cstdint>
#include <functional>
#include <string>

#include "logger/logger.hpp"
#include "types.hpp"
#include "utils/hasher.hpp"

SpeedGbps parse_speed(const std::string& throughput);
TimeNs parse_latency(const std::string& latency);
SizeByte parse_buffer_size(const std::string& buffer_size);

std::unique_ptr<sim::IHasher> parse_hasher(const YAML::Node& node);

// Parses node[field_name] using value_parser if node contains field_name;
// Returns default_value otherwise
uint32_t parse_with_default(
    const YAML::Node& node, std::string_view field_name,
    std::function<uint32_t(const std::string&)> value_parser,
    uint32_t default_value);

template <typename T>
T parse_with_default(const YAML::Node& node, std::string_view field_name,
                     std::function<T(const std::string&)> value_parser,
                     T&& default_value) {
    if (!node[field_name]) {
        LOG_WARN(
            fmt::format("{} does not set ; use default value", field_name));
        return std::forward<T>(default_value);
    }
    return value_parser(node[field_name].as<std::string>());
}
