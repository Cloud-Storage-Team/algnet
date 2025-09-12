#include "parser/parse_utils.hpp"

#include <spdlog/fmt/fmt.h>

#include <stdexcept>

YAML::Node get_if_present(const YAML::Node &node, std::string_view field_name) {
    if (!node) {
        return node;
    }
    return node[field_name];
}

static std::pair<uint32_t, std::string> parse_value_unit(
    const std::string &value_with_unit) {
    const size_t unit_pos = value_with_unit.find_first_not_of("0123456789");
    if (unit_pos == std::string::npos) {
        throw std::runtime_error("Can not find unit : " + value_with_unit);
    }
    const uint32_t value = std::stoul(value_with_unit.substr(0, unit_pos));
    const std::string unit = value_with_unit.substr(unit_pos);
    return std::make_pair(value, unit);
}

SpeedGbps parse_speed(const std::string &throughput) {
    auto [value, unit] = parse_value_unit(throughput);
    if (unit == "Mbps") {
        return Speed<MBit, Second>(value);
    }
    if (unit == "Gbps") {
        return SpeedGbps(value);
    }
    throw std::runtime_error("Unsupported throughput unit: " + unit);
}

TimeNs parse_time(const std::string &time) {
    auto [value, unit] = parse_value_unit(time);
    if (unit == "ns") {
        return TimeNs(value);
    }
    if (unit == "us") {
        return Time<Microsecond>(value);
    }
    if (unit == "ms") {
        return Time<Millisecond>(value);
    }
    if (unit == "s") {
        return Time<Second>(value);
    }
    throw std::runtime_error("Unsupported time unit: " + unit);
}

SizeByte parse_size(const std::string &size) {
    auto [value, unit] = parse_value_unit(size);
    if (unit == "b") {
        return Size<Bit>(value);
    }
    if (unit == "B") {
        return SizeByte(value);
    }
    if (unit == "Kb") {
        return Size<KBit>(value);
    }
    if (unit == "KB") {
        return Size<KByte>(value);
    }
    if (unit == "Mb") {
        return Size<MBit>(value);
    }
    if (unit == "MB") {
        return Size<MByte>(value);
    }
    if (unit == "Gb") {
        return Size<GBit>(value);
    }
    if (unit == "GB") {
        return Size<GByte>(value);
    }
    throw std::runtime_error("Unsupported buffer size unit: " + unit);
}
