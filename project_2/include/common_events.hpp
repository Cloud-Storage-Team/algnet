#pragma once

#include "event.hpp"
#include "packet.hpp"
#include "network_element.hpp"
#include "server.hpp"

class ProcessPacketEvent: public Event {
public:
    explicit ProcessPacketEvent(std::shared_ptr<NetworkElement> next_network_element, PacketHeader& packet, std::uint64_t event_time, std::uint32_t priority = 0);
    void perform_action(std::priority_queue<std::shared_ptr<Event>, std::vector<std::shared_ptr<Event>>, EventComparator>& events) override final;

    PacketHeader packet;
    std::shared_ptr<NetworkElement> next_network_element;
};
