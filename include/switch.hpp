#pragma once

#include <vector>
#include <deque>
#include "packet.hpp"
#include "network_element.hpp"

/**
 * @brief Switch abstraction
 * 
 * @todo Think about situations when we dont know when packet will be sent next time
 * 
 */
class NetworkSwitch: public RoutingNetworkElement {
protected:
    // TODO: add id (may be useful for more complicated topologies)
    // Packet queue inside switch
    // TODO: use dequeue instead of queue
    std::deque<PacketHeader> buffer{};
    std::uint64_t max_buffer_size_bit = 81920;
    std::uint64_t current_buffer_size_bit = 0;
    
public:
    /**
     * @brief creates bottle neck in network and process packets
     * 
     */
    virtual void ReceivePacket(std::uint64_t current_time_ns, PacketHeader& packet, std::priority_queue<std::shared_ptr<Event>, std::vector<std::shared_ptr<Event>>, EventComparator>& all_events) override = 0;
    virtual ~NetworkSwitch() {}
};
