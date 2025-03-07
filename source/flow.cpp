#include <event.hpp>
#include <flow.hpp>
#include <scheduler.hpp>

namespace sim {

Flow::Flow(ISender *a_src, IReceiver *a_dest, uint32_t a_packet_size)
    : m_src(a_src), m_dest(a_dest), m_packet_size(a_packet_size) {}

void Flow::schedule_packet_generation(std::uint32_t time) {
    Generate generate_event(this);
    generate_event.time = time;
    Scheduler::get_instance().add(generate_event);
}

void Flow::generate_packet() {
    sim::Packet packet;
    packet.type = sim::PacketType::DATA;
    packet.size = m_packet_size;
    packet.flow = this;
    m_src->enqueue_packet(packet);
}

}  // namespace sim