#pragma once

#include <vector>
#include <queue>
#include "packet.hpp"
#include "utils.hpp"

//Switch abstraction
class NetworkSwitch: public RoutingNetworkElement {
protected:
    // TODO: add id (may be useful for more complicated topologies)
    // Packet queue inside switch
    std::priority_queue<PacketHeader> buffer{};
    std::uint8_t max_buffer_size = 64;
    
public:
    virtual void ReceivePacket(std::uint64_t current_time_ns, PacketHeader& packet, PriorityQueueWrapper& packets_wrapped) override = 0;
    virtual ~NetworkSwitch() {}
};
