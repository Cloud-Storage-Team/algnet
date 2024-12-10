#pragma once

#include "packet.hpp"
#include "network_element.hpp"

#include <cstdint>
#include <memory>

class NetworkElement;

/**
 * @brief Stores packet and next network element in route
 * 
 */
class RoutingPacket {
private:
    // TODO: store time of passing last network element here and use it instead of PacketHeader::sending_time
    PacketHeader packet;
    std::shared_ptr<NetworkElement> next_network_element;
public:
    RoutingPacket() = default;
    RoutingPacket(PacketHeader& packet, std::shared_ptr<NetworkElement> next_network_element);

    PacketHeader GetPacket() const;
    std::shared_ptr<NetworkElement> GetNext() const;

    bool operator<(const RoutingPacket& other) const;
};
