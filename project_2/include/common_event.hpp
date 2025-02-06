// #pragma once

// #include "event.hpp"
// #include "packet.hpp"
// #include "network_element.hpp"
// #include "server.hpp"
// #include "express_pass_server.hpp"

// class ProcessPacketEvent: public Event {
// public:
//     explicit ProcessPacketEvent(std::shared_ptr<NetworkElement> next_network_element, PacketHeader& packet, std::uint64_t event_time, std::uint32_t priority = 0);
//     void perform_action(EventQueue& events) override final;

//     PacketHeader packet;
//     std::shared_ptr<NetworkElement> next_network_element;
// };

// class CreatePacketEvent: public Event {
// public:
//     explicit CreatePacketEvent(std::shared_ptr<ServerBase> host, std::uint64_t event_time, std::uint32_t priority = 0);
//     void perform_action(EventQueue& events) override final;

//     std::shared_ptr<ServerBase> host;
// };

// // Required only for ExpressPass

// class CreateCreditByFlowEvent: public Event {
// public:
//     explicit CreateCreditByFlowEvent(std::shared_ptr<ExpressPassReceiver> host, std::uint64_t flow_id, std::uint64_t event_time, std::uint32_t priority = 0);
//     virtual ~CreateCreditByFlowEvent() {};

//     void perform_action(EventQueue& events) override final;

//     std::uint64_t flow_id;
//     std::shared_ptr<ExpressPassReceiver> host;
// };
