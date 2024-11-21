#include "NetworkSimulator.hpp"

NetworkSimulator::NetworkSimulator(const std::vector<std::uint32_t>& distances_to_receiver) {
    for (std::uint32_t distance: distances_to_receiver) {
        servers.emplace_back(++last_given_server_id, distance);
    }
    std::uint64_t server_receiver_id = 0;
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
        events.emplace_back(server, event_type::SEND_DATA, server.GetCWNDSize());
        // Send packets
        server.SendPackets(packets);
    }
}

void NetworkSimulator::PacketsReceiving() {
    std::uint64_t received_data_bytes = 0;

    // Create acknowledgements
    while (!packets.empty()) {
        if (received_data_bytes >= NetworkSimulator::bandwidth_bytes) {
            break;
        }
        received_data_bytes += NetworkSimulator::packet_size_bytes;
        // Add ACK event
        events.emplace_back(server_receiver, event_type::ACKNOWLEDGEMENT, 1);

        // Index of server-sender in vector
        std::uint32_t sending_server_id = packets.top().GetSenderID() - 1;

        // Send an ACK
        std::uint64_t ack_estimated_delivery_time_ns = packets.top().GetEstimatedDeliveryTime() +
                                                       servers[sending_server_id].GetPacketTransmissionTime();
        servers[sending_server_id].AddACK(ACK(packets.top().GetSenderID(), ack_estimated_delivery_time_ns));

        packets.pop();
    }
}

void NetworkSimulator::StartSimulation(std::uint32_t simulation_iterations) {
    bool is_first_iteration = true;

    for (std::uint32_t i = 0; i < simulation_iterations; ++i) {
        // Servers send packets
        PacketsSending(is_first_iteration);
        // Server-receiver receives packets and sends ACKs
        PacketsReceiving();


        // Calculate mean CWND size in packets
        std::uint64_t cwnd_sizes_sum = 0;
        for (const auto& server: servers) {
            cwnd_sizes_sum += server.GetCWNDSize();
        }
        std::uint64_t mean_cwnd_size = cwnd_sizes_sum / servers.size();

        // Print queue size and mean CWND size
        std::cout << i << " " << packets.size() << " " << mean_cwnd_size << std::endl;
        is_first_iteration = false;
    }
}

std::ostream& operator<<(std::ostream& out, const NetworkSimulator& simulator) {
    out << "==========Network simulator configuration==========\n"
        << "Bandwidth: " << NetworkSimulator::bandwidth_bytes << " bytes.\n"
        << "Max data transmission frequency: " << NetworkSimulator::max_data_rate_bytes << " bytes.\n"
        << "Packet size: " << NetworkSimulator::packet_size_bytes << " bytes.\n"
        << "Server receiver ID: " << simulator.server_receiver.GetID() << ".\n"
        << "Sending servers:\n";
    for (auto& server: simulator.servers) {
        out << "ID: " << server.GetID() << ". \tData rate: " << server.GetDataRate()
            << " Gbps. \tCWND size: " << server.GetCWNDSize() << " packets.\n";
    }

    return out;
}
