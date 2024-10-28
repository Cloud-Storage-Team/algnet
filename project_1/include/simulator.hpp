#pragma once

#include "utils.hpp"

#include <vector>
#include <cstdint>
#include <queue>

class NetworkSimulator {
private:
    std::uint32_t bandwidth = 52428800;
    std::uint32_t data_transmission_frequency_bytes = 52428800;
    std::uint32_t data_transmission_frequency_packages = 51200;
    std::uint32_t package_size = 1024;

    // Number of sending servers
    std::uint32_t servers_number;

    // Contains all sending servers 
    std::vector<ServerSender> servers{};
    ServerBase server_receiver{};

    // Sorts packages by travel time
    std::priority_queue<PackageHeader> packages{};
    std::queue<PackageHeader> sliding_window{};
    
    // Contains all events 
    std::vector<Event> events{};

public:
    explicit NetworkSimulator(const std::vector<std::uint32_t>&);
    ~NetworkSimulator() = default;

    void AddServer(std::uint32_t);
    void StartSimulation();

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
