#pragma once
#include <spdlog/fmt/fmt.h>

#include <cstdint>
#include <functional>
#include <string>

#include "logger/logger.hpp"
#include "parser/config_reader/config_node.hpp"
#include "types.hpp"

SpeedGbps parse_speed(const std::string& throughput);
TimeNs parse_time(const std::string& time);
SizeByte parse_size(const std::string& buffer_size);

YAML::Node get_if_present(const YAML::Node& node, std::string_view field_name);

// Parses node[field_name].as<T> if node contains field_name;
// Returns default_value otherwise;
// Commonly used for simple types like std::string, int, double etc
template <typename T>
T simple_parse_with_default(const sim::ConfigNode& node,
                            std::string_view field_name, T default_value) {
    static_assert(std::is_copy_constructible_v<T>,
                  "T must be copy constructible");

    if (sim::ConfigNodeExpected value_node = node[field_name]; value_node) {
        return value_node.value().as<T>().value_or_throw();
    }
    return default_value;
}
