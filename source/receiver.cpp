#include "receiver.hpp"

#include <iostream>

#include "event.hpp"
#include "flow.hpp"
#include "link.hpp"
#include "packet.hpp"
#include "scheduler.hpp"

namespace sim {

// std::uint32_t Receiver::process() {
//     Link* current_inlink = (*m_next_inlink);
//     std::uint32_t total_processing_time = 0;

//     // TODO: maybe move to device class method
//     if (++m_next_inlink == m_inlinks.end()) {
//         m_next_inlink = m_inlinks.begin();
//     }

//     Packet data_packet = current_inlink->get_packet();

//     // processing...
//     // total_processing_time += processing_time;

//     Packet ack = {PacketType::ACK, 1, data_packet.flow};
//     Link* link_to_src = m_routing_table[data_packet.flow->get_source()];
//     // Not sure if we want to send ack before processing or after it
//     link_to_src->schedule_arrival(ack);

//     return total_processing_time;
// }

}  // namespace sim
