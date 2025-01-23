#pragma once

#include <vector>
#include <deque>
#include "packet.hpp"
#include "network_element.hpp"

class NetworkSwitch: public NetworkElement {
protected:
    // TODO: add id (may be useful for more complicated topologies)
    // Packet queue inside switch
    std::deque<PacketHeader> buffer{};
    std::uint64_t max_buffer_size_bit = 81920;
    std::uint64_t current_buffer_size_bit = 0;
    
public:
    virtual void ReceivePacket(std::uint64_t current_time_ns, PacketHeader& packet, std::priority_queue<std::shared_ptr<Event>, std::vector<std::shared_ptr<Event>>, EventComparator>& all_events) override = 0;
    virtual ~NetworkSwitch() {}
};
