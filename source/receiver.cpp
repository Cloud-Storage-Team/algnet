#include "receiver.hpp"
#include "event.hpp"
#include "scheduler.hpp"
#include "packet.hpp"
#include "link.hpp"
#include "flow.hpp"

#include <iostream>

namespace sim {


void Receiver::process(std::uint32_t start_time) {
    Link* current_inlink = (*m_next_inlink);
    std::uint32_t current_time = start_time;

    // TODO: maybe move to device class method
    if (++m_next_inlink == m_inlinks.end()) {
        m_next_inlink = m_inlinks.begin();
    }

    Packet data_packet = current_inlink->get_packet();

    // processing...
    // current_time += processing_time;

    Process new_process_event = Process(this);
    new_process_event.time = current_time;
    Scheduler::get_instance().add(new_process_event);

    Packet ack = {PacketType::ACK, 1, data_packet.flow};
    Link* link_to_src = m_routing_table[data_packet.flow->get_source()];
    // Not sure if we want to send ack before processing or after it
    link_to_src->schedule_arrival(ack);
}

}  // namespace sim
