#pragma once

#include "utils.hpp"

#include <vector>
#include <cstdint>
#include <queue>
#include <chrono>

class NetworkSimulator {
private:
    std::uint32_t bandwidth = 52428800;
    std::uint32_t data_transmission_frequency_bytes = 52428800;
    std::uint32_t data_transmission_frequency_packets = 51200;
    std::uint32_t packet_size = 1024;

    // Contains all sending servers 
    std::vector<ServerSender> servers{};
    ServerBase server_receiver{};

    // Sorts packets by travel time
    std::priority_queue<PacketHeader> packets{};
    
    // Contains all events 
    std::vector<Event> events{};

    void PacketsSending(bool);
    void PacketsReceiving();

public:

    explicit NetworkSimulator(const std::vector<std::uint32_t>&);
    ~NetworkSimulator() = default;

    void StartSimulation(std::uint32_t);

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
