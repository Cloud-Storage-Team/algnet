#pragma once

#include "Flow.hpp"

#include <cstdint>
#include <vector>


/**
 *  Implementation of a network simulator.
 */
class NetworkSimulator {
public:
    explicit NetworkSimulator(const std::vector<std::uint32_t>& distances);
    ~NetworkSimulator();

    /**
     * Start simulation.
     */
    void Run();

    /**
     * Size of a packet in bytes.
     */
    static const std::uint32_t packet_size_bytes = 1024;

    /**
     * Size of a receiver's bandwidth in bytes.
     */
    static const std::uint64_t bandwidth_bytes = 6'250'000'000;
private:
    Flow* flow;
};
