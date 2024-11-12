#pragma once

#include "server.hpp"
#include "event.hpp"
#include "packet.hpp"

#include <vector>
#include <cstdint>
#include <queue>
#include <chrono>

class NetworkSimulator {
private:
    std::uint64_t bandwidth_bytes = 6'250'000'000;
    std::uint64_t data_transmission_frequency_bytes = 6'250'000'000;
    std::uint64_t data_transmission_frequency_packets = 6'250'000;
    std::uint32_t packet_size_bytes = 1024;

    // Contains all sending servers 
    std::vector<ServerSender> servers{};
    ServerBase server_receiver{};

    // Sorts packets by travel time
    std::priority_queue<PacketHeader> packets{};
    
    // Contains all events 
    std::vector<Event> events{};

    void PacketsSending(bool is_first_iteration);
    void PacketsReceiving();

public:

    explicit NetworkSimulator(const std::vector<std::uint32_t>& distances_to_receiver);
    ~NetworkSimulator() = default;

    void StartSimulation(std::uint32_t simulation_time_sec);

    // New ID = last given ID + 1
    template<typename T>
    std::uint64_t GenerateNewID(const std::vector<T>& data) {
        std::uint64_t new_id = 1;
        if (!data.empty()) {
            new_id = data.back().GetID() + 1;
        }
        return new_id;
    }

    friend std::ostream& operator<<(std::ostream& out, const NetworkSimulator& simulator);
};
