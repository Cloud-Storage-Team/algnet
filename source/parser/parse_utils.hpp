#pragma once
#include <yaml-cpp/yaml.h>

#include <cstdint>
#include <functional>
#include <string>

#include "logger/logger.hpp"
#include "utils/hasher.hpp"

uint32_t parse_throughput(const std::string& throughput);
uint32_t parse_latency(const std::string& latency);
uint32_t parse_buffer_size(const std::string& buffer_size);

// Parses node[field_name] using value_parser if node contains filed_name;
// Returns default_value otherwise
uint32_t parse_with_default(
    const YAML::Node& node, std::string_view field_name,
    std::function<uint32_t(const std::string&)> value_parser,
    uint32_t default_value);

std::unique_ptr<sim::IHasher> parse_hasher(const YAML::Node& node);
