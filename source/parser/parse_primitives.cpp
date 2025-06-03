#include "parse_primitives.hpp"

#include <stdexcept>

uint32_t parse_throughput(const std::string &throughput) {
    const size_t unit_pos = throughput.find_first_not_of("0123456789");
    if (unit_pos == std::string::npos) {
        throw std::runtime_error("Invalid throughput: " + throughput);
    }
    const uint32_t value = std::stoul(throughput.substr(0, unit_pos));
    const std::string unit = throughput.substr(unit_pos);
    if (unit == "Gbps") {
        return value;
    }
    if (unit == "Mbps") {
        return value / 1000;  // Convert to Gbps
    }
    throw std::runtime_error("Unsupported throughput unit: " + unit);
}

uint32_t parse_latency(const std::string &latency) {
    const size_t unit_pos = latency.find_first_not_of("0123456789");
    if (unit_pos == std::string::npos) {
        throw std::runtime_error("Invalid latency: " + latency);
    }
    const uint32_t value = std::stoul(latency.substr(0, unit_pos));
    const std::string unit = latency.substr(unit_pos);
    if (unit == "ns") {
        return value;
    }
    throw std::runtime_error("Unsupported latency unit: " + unit);
}

uint32_t parse_buffer_size(const std::string &buffer_size) {
    const size_t unit_pos = buffer_size.find_first_not_of("0123456789");
    if (unit_pos == std::string::npos) {
        throw std::runtime_error("Invalid buffer_size: " + buffer_size);
    }
    const uint32_t value = std::stoul(buffer_size.substr(0, unit_pos));
    const std::string unit = buffer_size.substr(unit_pos);
    if (unit == "B") {
        return value;
    }
    throw std::runtime_error("Unsupported latency unit: " + unit);
}
