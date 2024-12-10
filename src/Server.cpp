#include "Server.hpp"
#include "Event.hpp"

Server::Server():
    NetworkDevice(DeviceType::Server) {
    id = NetworkDevice::last_given_device_id++;
}

void Server::Send(std::shared_ptr<NetworkDevice> receiver, std::uint32_t distance_ns) {
    std::uint32_t packets_number = 10;
    for (std::uint32_t i = 0; i < packets_number; ++i) {
        // Create packet
        Packet p(current_time_ns);
        sent_packets.push_back(p);

        // Create event (i. e. send packet)
        receiver->PushStorage(Event(&sent_packets.back(), current_time_ns + distance_ns));

        // Update sender local time
        current_time_ns += distance_ns;
    }
}

const Event& Server::TopStorage() {
    return ingress_events.front();
}

void Server::PopStorage() {
    ingress_events.pop_front();
}

void Server::PushStorage(const Event &event) {
    ingress_events.push_back(event);
}