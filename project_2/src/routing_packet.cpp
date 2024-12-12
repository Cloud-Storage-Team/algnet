#include "packet.hpp"
#include "network_element.hpp"

#include <cstdint>
#include <memory>

RoutingPacket::RoutingPacket(PacketHeader& packet, std::shared_ptr<NetworkElement> next_network_element):
    packet(packet),
    next_network_element(next_network_element) {}

PacketHeader RoutingPacket::GetPacket() const {
    return packet;
}

std::shared_ptr<NetworkElement> RoutingPacket::GetNext() const {
    return next_network_element;
}

bool RoutingPacket::operator<(const RoutingPacket& other) const {
    return this->packet < other.packet;
}