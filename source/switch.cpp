// #include "switch.hpp"

// #include "link.hpp"

// namespace sim {

// Switch::Switch(DeviceType a_type) : Device(a_type) {}

// void Switch::process() {
//     Link* link = next_inlink();
//     Packet packet = link->get_packet();
//     Device* destination = packet.flow->get_destination();
//     Link* next_link = m_routing_table[destination];
//     next_link->schedule_arrival(packet);
// }

// }  // namespace sim
