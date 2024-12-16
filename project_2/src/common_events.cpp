#include "common_events.hpp"

#include <string>
#include <queue>
#include <iostream>

ProcessPacketEvent::ProcessPacketEvent(std::shared_ptr<NetworkElement> next_network_element, PacketHeader& packet, std::uint64_t event_time, std::uint32_t priority):
    Event(event_time, priority),     
    packet(packet),
    next_network_element(next_network_element) {}

void ProcessPacketEvent::perform_action(std::priority_queue<std::shared_ptr<Event>, std::vector<std::shared_ptr<Event>>, EventComparator>& events) {
    next_network_element->ReceivePacket(event_time, packet, events);
}

CreatePacketEvent::CreatePacketEvent(std::shared_ptr<ServerBase> host, std::uint64_t event_time, std::uint32_t priority):
    Event(event_time, priority),
    host(host) {}

void CreatePacketEvent::perform_action(std::priority_queue<std::shared_ptr<Event>, std::vector<std::shared_ptr<Event>>, EventComparator>& events) {
    host->SendPackets(event_time, events);
}
