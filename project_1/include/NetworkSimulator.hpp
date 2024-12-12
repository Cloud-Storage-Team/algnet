#pragma once

#include "ServerSender.hpp"
#include "Event.hpp"
#include "PacketHeader.hpp"

#include <vector>
#include <cstdint>
#include <queue>
#include <chrono>

class NetworkSimulator {
public:
    explicit NetworkSimulator(const std::vector<std::uint32_t>& distances_to_receiver);
    ~NetworkSimulator() = default;

    void StartSimulation(std::uint32_t simulation_time_sec);

    friend std::ostream& operator<<(std::ostream& out, const NetworkSimulator& simulator);

    static const std::uint64_t bandwidth_bytes = 6'250'000'000;
    static const std::uint64_t data_transmission_frequency_bytes = 6'250'000'000;
    static const std::uint64_t data_transmission_frequency_packets = 6'250'000;
    static const std::uint32_t packet_size_bytes = 1024;

    // New ID = last given ID + 1
    std::uint64_t last_given_server_id = 0;

    // Contains all sending servers 
    std::vector<ServerSender> servers{};
    ServerBase server_receiver{};

    // Sorts packets by travel time
    std::priority_queue<PacketHeader> packets{};
    
    // Contains all events 
    std::vector<Event> events{};

    void PacketsSending(bool is_first_iteration);
    void PacketsReceiving();
};
