#include "simulator.hpp"

#include <chrono>

NetworkSimulator::NetworkSimulator(std::uint32_t servers_number) : servers_number(servers_number) {
    std::uint32_t server_receiver_id = 0;
    std::uint32_t distance_to_receiver = 0;
    server_receiver = Server(server_receiver_id, distance_to_receiver);
}

void NetworkSimulator::AddServer(std::uint32_t distance) {
    if (servers.size() < servers_number) {
        std::uint32_t new_server_id = 1;
        if (!servers.empty()) {
            new_server_id = servers.back().GetID() + 1;
        }
        servers.emplace_back(new_server_id, distance);
    }
}

void NetworkSimulator::StartSimulation(std::vector<std::uint32_t>& distances_to_receiver) {
    if (distances_to_receiver.size() > servers_number) {
        distances_to_receiver.resize(servers_number);
    }
    for (std::uint32_t distance: distances_to_receiver) {
        AddServer(distance);
    }
    std::cout << "Simulator setup successfully completed.\n";

    std::uint32_t new_event_id = 1;
    if (!events.empty()) {
        new_event_id = events.back().GetID() + 1;
    }

    std::uint32_t test_packages_number = 5;
    for (Server& server: servers) {
        events.emplace_back(new_event_id, server, SEND_DATA, test_packages_number);

        for (std::uint32_t i = 0; i < test_packages_number; ++i) {
            auto duration = std::chrono::high_resolution_clock::now().time_since_epoch();
            auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
            auto current_time_ms = static_cast<std::uint32_t>(milliseconds);
            packages.emplace(server.GetID(), server_receiver.GetID(), current_time_ms, test_packages_number, i);
        }

        std::cout << "[EVENT] ID: " << new_event_id
                  << ". Initiator ID: " << server.GetID()
                  << ". Type: SEND_DATA. Package number: "
                  << test_packages_number << ".\n";
        new_event_id++;
    }

    auto duration = std::chrono::high_resolution_clock::now().time_since_epoch();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    auto current_time_ms = static_cast<std::uint32_t>(milliseconds);
    std::uint32_t time_start = current_time_ms;

    std::uint32_t sliding_window_duration_ms = 1000;

    while (!packages.empty() && time_start - packages.top().GetSendingTime() < sliding_window_duration_ms) {
        sliding_window.push(packages.top());
        packages.pop();
    }

    while (!sliding_window.empty()) {
        events.emplace_back(new_event_id, server_receiver, ACKNOWLEDGEMENT, 1);

        std::cout << "[EVENT] ID: " << new_event_id
                  << ". Initiator ID: " << server_receiver.GetID()
                  << ". Receiver ID: " << sliding_window.front().GetSenderID()
                  << ". Type: ACKNOWLEDGEMENT. Package number: "
                  << sliding_window.front().GetPackageIndex() << ".\n";
        new_event_id++;

        sliding_window.pop();
    }
}

void NetworkSimulator::PrintConfiguration() const {
    std::cout << "=====Network simulator configuration=====\n"
              << "Bandwidth: " << bandwidth << " bytes.\n"
              << "Data transmission frequency: " << data_transmission_frequency_bytes << " bytes.\n"
              << "Package size: " << package_size << " bytes.\n"
              << "Server receiver ID: " << server_receiver.GetID() << ".\n"
              << "Sending servers:\n";
    for (auto& server: servers) {
        std::cout << "ID: " << server.GetID() << ". Distance to receiver: " << server.GetDistance() << " ms.\n";
    }
}
