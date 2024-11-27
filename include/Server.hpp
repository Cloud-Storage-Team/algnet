#pragma once

#include "Packet.hpp"

#include <cstdint>
#include <vector>
#include <queue>

/**
 * Server receiver
 *
 * The main task of ServerBase -- receive packets and send ACKs
 */
struct ServerBase {
    explicit ServerBase(std::uint32_t id);
    ServerBase() = default;
    virtual ~ServerBase() = default;

    /**
     * Get ID of server
     * @return std::uint32_t -- server's ID
     */
    std::uint32_t GetID() const;

    /**
     * Get server's current time in nanoseconds
     * @return std::uint64_t -- server's current time in nanoseconds
     */
    std::uint64_t GetCurrentTime() const;

    /**
     * Replace server's current time with a new value
     * @param new_time -- new value of time in nanoseconds
     */
    void SetCurrentTime(std::uint64_t new_time);

    /**
     * Increase server's current time by delta_time nanoseconds
     * @param delta_time -- time step in nanoseconds
     */
    void IncreaseCurrentTime(std::uint64_t delta_time);
protected:
    std::uint32_t id;
    std::uint64_t current_time_ns = 0;
};

// Type for sending server, contains ID and distance to receiver
struct ServerSender : ServerBase {
    ServerSender(std::uint64_t id, std::uint32_t distance_ns);
    ServerSender() = default;
    ~ServerSender() = default;

    /**
     * Get server's CWND size in packets
     * @return std::uint64_t -- server's CWND size in packets
     */
    std::uint64_t GetCWNDSize() const;

    /**
     * Get server's distance to receiver in nanoseconds
     * @return std::uint32_t -- server's distance to receiver in nanoseconds
     */
    std::uint32_t GetDistance() const;
private:
    std::uint32_t distance_ns;
    std::uint64_t cwnd_size_packets = 1;
};
