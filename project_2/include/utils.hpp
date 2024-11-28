#pragma once

#include "routing_packet.hpp"

#include <iostream>
#include <queue>

class RoutingPacket;
/**
 * @brief queue with limited functionality
 * 
 * @remark provides only method push of packet queue
 * 
 */
class PriorityQueueWrapper {
private:
    std::priority_queue<RoutingPacket>* packets;
public:
    PriorityQueueWrapper(std::priority_queue<RoutingPacket>* packets): packets(packets) {}

    /**
     * @brief provides push method to allow appending new elements to queue
     * 
     * @param packet 
     */
    void push(const RoutingPacket& packet) {
        // std::cout << "Added: " << packets->size() << std::endl;
        packets->push(packet);
        // std::cout << "Added: " << packet.GetDestinationID() << " " << packet.GetSendingTime() << std::endl;
    }
};