#pragma once

#include <iostream>
#include <queue>

#include "routing_packet.hpp"

class RoutingPacket;

class PriorityQueueWrapper {
  private:
    std::priority_queue<RoutingPacket> *packets;

  public:
    PriorityQueueWrapper(std::priority_queue<RoutingPacket> *packets)
        : packets(packets) {
    }

    void push(const RoutingPacket &packet) {
        packets->push(packet);
        // std::cout << "Added: " << packet.GetDestinationID() << " " <<
        // packet.GetSendingTime() << std::endl;
    }
};