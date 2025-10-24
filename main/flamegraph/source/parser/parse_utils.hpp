#pragma once
#include <spdlog/fmt/fmt.h>

#include <cstdint>
#include <functional>
#include <regex>
#include <string>

#include "logger/logger.hpp"
#include "parser/config_reader/config_node.hpp"
#include "types.hpp"
#include "utils/str_expected.hpp"

namespace sim {

utils::StrExpected<SpeedGbps> parse_speed(const std::string& throughput);
utils::StrExpected<TimeNs> parse_time(const std::string& time);
utils::StrExpected<SizeByte> parse_size(const std::string& size);

SpeedGbps parse_speed(const ConfigNode& throughput_node);
TimeNs parse_time(const ConfigNode& time_node);
SizeByte parse_size(const ConfigNode& size_node);
std::regex parse_regex(const ConfigNode& regex_node);

// Parses node[field_name].as<T> if node contains field_name;
// Returns default_value otherwise;
// Commonly used for simple types like std::string, int, double etc
template <typename T>
T simple_parse_with_default(const sim::ConfigNode& node,
                            std::string_view field_name, T default_value) {
    static_assert(std::is_copy_constructible_v<T>,
                  "T must be copy constructible");

    if (sim::ConfigNodeExpected value_node = node[field_name]; value_node) {
        return value_node.value().as_or_throw<T>();
    }
    return default_value;
}

}  // namespace sim