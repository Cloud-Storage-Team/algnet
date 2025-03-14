#include "flow.hpp"

#include "scheduler.hpp"

namespace sim {

IFlow::IFlow(ISender *a_src, IReceiver *a_dest, uint32_t a_packet_size)
    : m_src(a_src), m_dest(a_dest), m_packet_size(a_packet_size) {}

void IFlow::schedule_packet_generation(std::uint32_t time) {
    auto generate_event_ptr = std::make_unique<Generate>(Generate(this));
    generate_event_ptr->time = time;
    Scheduler::get_instance().add(std::move(generate_event_ptr));
}

void IFlow::generate_packet() {
    sim::Packet packet;
    packet.type = sim::PacketType::DATA;
    packet.size = m_packet_size;
    packet.flow = this;
    m_src->enqueue_packet(packet);
}

}  // namespace sim