#include "simulator.hpp"

NetworkSimulator::NetworkSimulator(const std::vector<std::uint32_t>& distances_to_receiver) {
    for (std::uint32_t distance: distances_to_receiver) {
        servers.emplace_back(GenerateNewID(servers), distance);
    }
    std::uint32_t server_receiver_id = 0;
    server_receiver = ServerBase(server_receiver_id);
}

void NetworkSimulator::PacketsSending(bool is_first_iteration) {
    // Each server send cwnd_size packets
    for (ServerSender& server: servers) {
        // Don't check ACKs on first iteration
        if (!is_first_iteration && !server.HandleACKs()) {
            // If server has no ACKs, it doesn't send anything
            continue;
        }
        // Create event
        events.emplace_back(server, SEND_DATA, server.GetCWNDSize());

        // Create packets, add them to priority_queue
        for (std::uint32_t i = 0; i < server.GetCWNDSize(); ++i) {
            // Calculate estimated delivering time of the packet
            std::uint32_t estimated_delivering_time = server.GetCurrentTime() + server.GetDistance();
            // Add packet to the priority_queue
            packets.emplace(server.GetID(), server.GetCurrentTime(), estimated_delivering_time);

            // Update server time
            server.IncreaseCurrentTime(server.GetDistance());
        }
        std::cout << events.back();
    }
}

void NetworkSimulator::PacketsReceiving() {
    std::uint32_t received_data_kb = 0;
    // Create acknowledgements
    while (!packets.empty()) {
        if (received_data_kb >= bandwidth) {
            break;
        }

        // Add ACK event
        events.emplace_back(server_receiver, ACKNOWLEDGEMENT, 1);

        // Index of server-sender in vector
        std::uint32_t sending_server_index = packets.top().GetSenderID() - 1;

        // Send an ACK
        servers[sending_server_index].AddACK(ACK(packets.top().GetSenderID(),
                                                 packets.top().GetEstimatedDeliveryTime() + servers[sending_server_index].GetDistance()));

        std::cout << events.back();
        received_data_kb += packet_size;
        packets.pop();
    }
}

void NetworkSimulator::StartSimulation() {
    bool is_first_iteration = true;

    // Servers send packets
    PacketsSending(is_first_iteration);
    // Server-receiver receives packets and sends ACKs
    PacketsReceiving();

    is_first_iteration = false;
}

std::ostream& operator<<(std::ostream& out, const NetworkSimulator& simulator) {
    out << "==========Network simulator configuration==========\n"
        << "Bandwidth: " << simulator.bandwidth << " bytes.\n"
        << "Data transmission frequency: " << simulator.data_transmission_frequency_bytes << " bytes.\n"
        << "Packet size: " << simulator.packet_size << " bytes.\n"
        << "Server receiver ID: " << simulator.server_receiver.GetID() << ".\n"
        << "Sending servers:\n";
    for (auto& server: simulator.servers) {
        out << "ID: " << server.GetID() << ". \tDistance to receiver: " << server.GetDistance()
            << " us. \tCWND size: " << server.GetCWNDSize() << " packets.\n";
    }

    return out;
}
