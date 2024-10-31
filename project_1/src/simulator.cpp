#include "simulator.hpp"

NetworkSimulator::NetworkSimulator(const std::vector<std::uint32_t>& distances_to_receiver) {
    for (std::uint32_t distance: distances_to_receiver) {
        servers.emplace_back(GenerateNewID(servers), distance);
    }
    std::uint32_t server_receiver_id = 0;
    server_receiver = ServerBase(server_receiver_id);
}

void NetworkSimulator::StartSimulation() {
    std::uint32_t test_packets_number = 5;

    // Each server send 5 packets
    for (ServerSender& server: servers) {
        // Create event
        events.emplace_back(GenerateNewID(events), server, SEND_DATA, test_packets_number);

        // Create packets, add them to priority_queue
        for (std::uint32_t i = 0; i < test_packets_number; ++i) {
            std::uint32_t current_time_us = 0;
            current_time_us += server.GetDistance();
            packets.emplace(server.GetID(), server_receiver.GetID(), current_time_us, i, server.GetDistance());
        }
        std::cout << events.back();
    }

    // Create acknowledgements
    while (!packets.empty()) {
        events.emplace_back(GenerateNewID(events), server_receiver, ACKNOWLEDGEMENT, 1);
        std::cout << events.back();
        packets.pop();
    }
}

std::ostream& operator<<(std::ostream& out, const NetworkSimulator& simulator) {
    out << "=====Network simulator configuration=====\n"
        << "Bandwidth: " << simulator.bandwidth << " bytes.\n"
        << "Data transmission frequency: " << simulator.data_transmission_frequency_bytes << " bytes.\n"
        << "Packet size: " << simulator.packet_size << " bytes.\n"
        << "Server receiver ID: " << simulator.server_receiver.GetID() << ".\n"
        << "Sending servers:\n";
    for (auto& server: simulator.servers) {
        out << "ID: " << server.GetID() << ". Distance to receiver: " << server.GetDistance() << " us.\n";
    }

    return out;
}
