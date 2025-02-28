#include "sender.hpp"
#include "link.hpp"
#include "event.hpp"
#include "scheduler.hpp"


namespace sim {

Sender::Sender(): Device(DeviceType::SENDER) {}

void Sender::enqueue_packet(Packet packet) {
    m_flow_buffer.push(packet);
}

void Sender::process() {
    // std::uint32_t current_time = start_time;

    // TODO: Handle empty m_inlinks
    Link* current_inlink = (*m_next_inlink);
    if (++m_next_inlink == m_inlinks.end()) {
        m_next_inlink = m_inlinks.begin();
    }

    // TODO: Handle absence of packet (return optional)
    Packet packet = current_inlink->get_packet();
    if (packet.type == PacketType::ACK) {
        packet.flow->update();
    }
    // current_time += processing_ack_time;

    if (m_flow_buffer.empty()) {
        return;
    }

    // TODO: wrap into some method
    Packet data_packet = m_flow_buffer.front();
    m_flow_buffer.pop();

    Device* destination = packet.flow->get_destination();
    Link* next_link = m_routing_table[destination];
    next_link->schedule_arrival(data_packet);
    // current_time += sending_data_time;


    Process new_process_event = Process(this);
    // new_process_event.time = current_time;
    Scheduler::get_instance().add(new_process_event);

}

}  // namespace sim
