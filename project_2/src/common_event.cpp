// #include "common_event.hpp"

// #include <string>
// #include <queue>
// #include <iostream>

// ProcessPacketEvent::ProcessPacketEvent(std::shared_ptr<NetworkElement> next_network_element, PacketHeader& packet, std::uint64_t event_time, std::uint32_t priority):
//     Event(event_time, priority),     
//     packet(packet),
//     next_network_element(next_network_element) {}

// void ProcessPacketEvent::perform_action(EventQueue& events) {
//     next_network_element->ReceivePacket(event_time, packet, events);
// }

// CreatePacketEvent::CreatePacketEvent(std::shared_ptr<ServerBase> host, std::uint64_t event_time, std::uint32_t priority):
//     Event(event_time, priority),
//     host(host) {}

// void CreatePacketEvent::perform_action(EventQueue& events) {
//     host->SendPackets(event_time, events);
// }

// CreateCreditByFlowEvent::CreateCreditByFlowEvent(std::shared_ptr<ExpressPassReceiver> host, std::uint64_t flow_id, std::uint64_t event_time, std::uint32_t priority):
//     Event(event_time, priority),
//     host(host),
//     flow_id(flow_id) {}

// void CreateCreditByFlowEvent::perform_action(EventQueue& events) {
//     host->SendCreditByFlow(flow_id, event_time, events);
// }