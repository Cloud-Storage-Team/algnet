#include "simulator.hpp"

std::uint64_t NetworkSimulator::id_to_give = 0;

NetworkSimulator::NetworkSimulator(std::vector<std::shared_ptr<ServerBase>>& senders, std::shared_ptr<ServerBase> receiver, std::shared_ptr<NetworkSwitch> n_switch, std::uint64_t simulation_duration_ns):
    receiver(receiver),
    n_switch(n_switch),
    simulation_duration_ns(simulation_duration_ns)
{
    std::shared_ptr<RoutingNetworkElement> routing_receiver = std::dynamic_pointer_cast<RoutingNetworkElement>(receiver);
    std::shared_ptr<RoutingNetworkElement> routing_switch = std::dynamic_pointer_cast<RoutingNetworkElement>(n_switch);
    auto recv_switch_conn = AddNewConnection(routing_receiver, routing_switch, 5);
    // std::cout << links.back() << " " << recv_switch_conn.reversed_link << std::endl;
    routing_switch->AddNewRout(receiver->GetID(), recv_switch_conn.reversed_link);

    for (std::shared_ptr<ServerBase> sender : senders) {
        ids_to_senders[sender->GetID()] = sender;
        std::shared_ptr<RoutingNetworkElement> routing_sender = std::dynamic_pointer_cast<RoutingNetworkElement>(sender);
        auto switch_sender_conn = AddNewConnection(routing_switch, routing_sender, 5);
        routing_switch->AddNewRout(sender->GetID(), switch_sender_conn.direct_link);
        routing_sender->AddNewRout(receiver->GetID(), switch_sender_conn.reversed_link);
        routing_receiver->AddNewRout(sender->GetID(), recv_switch_conn.direct_link);
    }
}

Connection NetworkSimulator::AddNewConnection(std::shared_ptr<RoutingNetworkElement> source, std::shared_ptr<RoutingNetworkElement> destination, std::uint64_t speed) {
    std::shared_ptr<NetworkElement> base_source = std::dynamic_pointer_cast<NetworkElement>(source);
    std::shared_ptr<NetworkElement> base_destination = std::dynamic_pointer_cast<NetworkElement>(destination);
    std::shared_ptr<Link> link = std::make_shared<Link>(Link(base_source, base_destination, speed));
    std::shared_ptr<Link> reverse_link = std::make_shared<Link>(Link(base_destination, base_source, speed));
    
    links.emplace_back(link);
    links.emplace_back(reverse_link);

    return Connection(std::move(link), std::move(reverse_link));
}

void NetworkSimulator::SendPackets(PriorityQueueWrapper& wrapped_packets) {
    // std::cout << "Sent packs" << std::endl;
    next_ask = 1e9;

    std::uint64_t possible_next_ask = receiver->SendPackets(current_time_ns, wrapped_packets);
    next_ask = std::min(next_ask, ((possible_next_ask == 0) ? next_ask : possible_next_ask));

    for (auto& id_and_sender : ids_to_senders) {
        possible_next_ask = id_and_sender.second->SendPackets(current_time_ns, wrapped_packets);   
        next_ask = std::min(next_ask, ((possible_next_ask == 0) ? next_ask : possible_next_ask));
    }
    // std::cout << next_ask << std::endl;
}

void NetworkSimulator::ProcessNextPacket(PriorityQueueWrapper& wrapped_packets) {
    // std::cout << "Processed pac" << std::endl;
    RoutingPacket r_packet = packets.top();
    // std::cout << "Removed: " << packet.GetDestinationID() << " " << packet.GetSendingTime() << std::endl;
    packets.pop();
    PacketHeader packet = r_packet.GetPacket();
    r_packet.GetNext()->ReceivePacket(current_time_ns, packet, wrapped_packets);
}

void NetworkSimulator::StartSimulation() {
    // std::cout << "Started sim" << std::endl;
    PriorityQueueWrapper wrapped_packets(&packets);
    SendPackets(wrapped_packets);

    while (current_time_ns <= simulation_duration_ns) {
        // std::cout << current_time_ns << std::endl;
        std::uint64_t new_time = next_ask;
        if (packets.empty()) {
            current_time_ns = new_time;
            SendPackets(wrapped_packets);
            continue;
        }

        std::uint64_t next_packet_time = packets.top().GetPacket().GetSendingTime();
        if (next_packet_time >= new_time) {
            current_time_ns = new_time;
            SendPackets(wrapped_packets);
        }

        if (next_packet_time <= new_time) {
            current_time_ns = next_packet_time;
            ProcessNextPacket(wrapped_packets);
        }
    }

    while (!packets.empty()) {
        std::uint64_t next_packet_time = packets.top().GetPacket().GetSendingTime();
        current_time_ns = next_packet_time;
        ProcessNextPacket(wrapped_packets);
    }
}
