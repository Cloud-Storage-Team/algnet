#pragma once

#include <cstdint>

struct INTHeader {
    std::uint64_t timestamp;
    std::uint64_t queue_length;
    std::uint64_t transmitted_bytes;
    std::uint64_t bandwidth_capacity;

    INTHeader(std::uint64_t timestamp, std::uint64_t queue_length, std::uint64_t transmitted_bytes, std::uint64_t bandwidth_capacity)
        : timestamp(timestamp), queue_length(queue_length), transmitted_bytes(transmitted_bytes), bandwidth_capacity(bandwidth_capacity) {}

    INTHeader() = default;
};
