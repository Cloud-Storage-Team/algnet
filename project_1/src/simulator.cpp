#include "simulator.hpp"

#include <chrono>

NetworkSimulator::NetworkSimulator(const std::vector<std::uint32_t>& distances_to_receiver) : servers_number(distances_to_receiver.size()) {
    for (std::uint32_t distance: distances_to_receiver) {
        AddServer(distance);
    }
    std::uint32_t server_receiver_id = 0;
    server_receiver = ServerBase(server_receiver_id);
}

void NetworkSimulator::AddServer(std::uint32_t distance_to_receiver) {
    if (servers.size() < servers_number) {
        servers.emplace_back(GenerateNewID(servers), distance_to_receiver);
    }
}

void NetworkSimulator::StartSimulation() {
    std::uint32_t test_packages_number = 5;

    // Each server send 5 packages
    for (ServerSender& server: servers) {
        // Create event
        events.emplace_back(GenerateNewID(events), server, SEND_DATA, test_packages_number);

        // Create packages, add them to priority_queue
        for (std::uint32_t i = 0; i < test_packages_number; ++i) {
            auto current_time_μs = GetCurrentTimeInMicroseconds();
            packages.emplace(server.GetID(), server_receiver.GetID(), current_time_μs, i);
        }
        std::cout << events.back();
    }

    auto current_time_μs = GetCurrentTimeInMicroseconds();
    std::uint32_t sliding_window_duration_μs = 1'000'000;

    // Collect packages arrived in last second (sliding_window_duration)
    while (!packages.empty() && current_time_μs - packages.top().GetSendingTime() < sliding_window_duration_μs) {
        sliding_window.push(packages.top());
        packages.pop();
    }

    // Create acknowledgements
    while (!sliding_window.empty()) {
        events.emplace_back(GenerateNewID(events), server_receiver, ACKNOWLEDGEMENT, 1);
        std::cout << events.back();
        sliding_window.pop();
    }
}

std::ostream& operator<<(std::ostream& out, const NetworkSimulator& simulator) {
    out << "=====Network simulator configuration=====\n"
        << "Bandwidth: " << simulator.bandwidth << " bytes.\n"
        << "Data transmission frequency: " << simulator.data_transmission_frequency_bytes << " bytes.\n"
        << "Package size: " << simulator.package_size << " bytes.\n"
        << "Server receiver ID: " << simulator.server_receiver.GetID() << ".\n"
        << "Sending servers:\n";
    for (auto& server: simulator.servers) {
        out << "ID: " << server.GetID() << ". Distance to receiver: " << server.GetDistance() << " μs.\n";
    }

    return out;
}
