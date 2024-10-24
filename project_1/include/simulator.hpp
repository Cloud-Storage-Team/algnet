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
    std::uint32_t servers_number;
    std::vector<Server> servers{};
    Server server_receiver{};

    std::priority_queue<PackageHeader> packages{};
    std::queue<PackageHeader> sliding_window{};
    std::vector<Event> events{};

public:
    explicit NetworkSimulator(std::uint32_t);
    ~NetworkSimulator() = default;

    void AddServer(std::uint32_t);
    void StartSimulation(std::vector<std::uint32_t>&);

    void PrintConfiguration() const;
};
