#include "parser/parse_utils.hpp"

#include <spdlog/fmt/fmt.h>

#include <stdexcept>

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

uint32_t parse_throughput(const std::string &throughput) {
    auto [value, unit] = parse_value_unit(throughput);
    if (unit == "Gbps") {
        return value;
    }
    if (unit == "Mbps") {
        return value / 1000;  // Convert to Gbps
    }
    throw std::runtime_error("Unsupported throughput unit: " + unit);
}

uint32_t parse_latency(const std::string &latency) {
    auto [value, unit] = parse_value_unit(latency);
    if (unit == "ns") {
        return value;
    }
    throw std::runtime_error("Unsupported latency unit: " + unit);
}

SizeByte parse_buffer_size(const std::string &buffer_size) {
    auto [value, unit] = parse_value_unit(buffer_size);
    if (unit == "B") {
        return SizeByte(value);
    }
    throw std::runtime_error("Unsupported latency unit: " + unit);
}
