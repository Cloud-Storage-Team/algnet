#include "Server.hpp"
#include "Event.hpp"
#include "NetworkSimulator.hpp"

#include <iostream>

Server::Server(DeviceType type):
        NetworkDevice(type) {
    id = NetworkDevice::last_given_device_id++;
    sent_packets = {};
}

void Server::Send(std::uint32_t flow_id) {
    std::tuple<std::uint32_t, std::uint64_t, std::uint32_t, std::uint64_t> neighbors_distances = NetworkSimulator::flows[flow_id]->FindAdjDevicesInPath(
            id);
    if (std::get<Flow::NextDist>(neighbors_distances) == 0 || std::get<Flow::PrevDist>(neighbors_distances) == 0) {
        std::cerr << "Error: failed to find neighbor devices on the path.\n";
        return;
    }
    if (type == DeviceType::ServerSender) {
        std::uint32_t receiver_id = std::get<Flow::NextID>(neighbors_distances);
        std::uint32_t distance_ns = std::get<Flow::NextDist>(neighbors_distances);

        std::uint32_t packets_number = 1;
        for (std::uint32_t i = 0; i < packets_number; ++i) {
            // Create packet
            std::shared_ptr<Packet> p = std::make_shared<Packet>(current_time_ns);
            sent_packets.push_back(p);

            // Create event (i. e. send packet)
            std::uint64_t link_last_process_time = NetworkSimulator::link_last_process_time_ns[{std::min(id, receiver_id), std::max(id, receiver_id)}];
            std::uint64_t event_delivery_time_ns = std::max(link_last_process_time, current_time_ns) + distance_ns;
            NetworkSimulator::event_scheduler.emplace(p, flow_id, receiver_id, event_delivery_time_ns, false);

            // Update link process time
            NetworkSimulator::link_last_process_time_ns[{std::min(id, receiver_id), std::max(id, receiver_id)}] = event_delivery_time_ns;

            // Update sender local time
            current_time_ns = event_delivery_time_ns;
        }
    }
    else if (type == DeviceType::ServerReceiver) {
        std::cout << "Packet received. Sending time: "
                  <<  NetworkSimulator::event_scheduler.top().packet->sending_time_ns
                  << ". Receiving time: " << NetworkSimulator::event_scheduler.top().delivery_time_ns << std::endl;

        std::uint32_t receiver_id = std::get<Flow::PrevID>(neighbors_distances);
        std::uint32_t distance_ns = std::get<Flow::PrevDist>(neighbors_distances);

        std::uint64_t link_last_process_time = NetworkSimulator::link_last_process_time_ns[{std::min(id, receiver_id), std::max(id, receiver_id)}];
        std::uint64_t event_delivery_time_ns = std::max(link_last_process_time, current_time_ns) + distance_ns;
        NetworkSimulator::event_scheduler.emplace(NetworkSimulator::event_scheduler.top().packet, flow_id, receiver_id, event_delivery_time_ns, true);

        // Update link process time
        NetworkSimulator::link_last_process_time_ns[{std::min(id, receiver_id), std::max(id, receiver_id)}] = event_delivery_time_ns;

        // Update sender local time
        current_time_ns = event_delivery_time_ns;
    }

}