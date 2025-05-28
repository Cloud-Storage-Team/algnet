#pragma once
#include <cstdint>
#include <string>

uint32_t parse_throughput(const std::string& throughput);
uint32_t parse_latency(const std::string& latency);
uint32_t parse_buffer_size(const std::string& buffer_size);
