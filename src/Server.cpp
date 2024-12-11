#include <iostream>
#include "Server.hpp"
#include "Event.hpp"
#include "NetworkSimulator.hpp"

Server::Server(DeviceType type):
    NetworkDevice(type) {
    id = NetworkDevice::last_given_device_id++;
    sent_packets = {};
}

void Server::Send(std::uint32_t flow_id) {
    if (type == DeviceType::ServerSender) {
        std::uint32_t packets_number = 10;
        auto [receiver_id, distance_ns] = NetworkSimulator::flows[flow_id]->FindNextDeviceInPath(id);
        if (distance_ns == 0) {
            std::cerr << "Error: receiver cannot send packets.\n";
            return;
        }
        for (std::uint32_t i = 0; i < packets_number; ++i) {
            // Create packet
            std::shared_ptr<Packet> p = std::make_shared<Packet>(current_time_ns);
            sent_packets.push_back(p);

            // Create event (i. e. send packet)
            std::uint64_t link_last_process_time = NetworkSimulator::link_last_process_time_ns[{std::min(id, receiver_id), std::max(id, receiver_id)}];
            std::uint64_t event_delivery_time_ns = std::max(link_last_process_time, current_time_ns) + distance_ns;
            NetworkSimulator::event_scheduler.emplace(p, flow_id, receiver_id, event_delivery_time_ns);

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
    }

}