#include "flow.hpp"
#include "scheduler.hpp"

namespace sim {

Flow::Flow(std::shared_ptr<ISender> a_src, std::shared_ptr<IReceiver> a_dest, uint32_t a_packet_size)
    : m_src(a_src), m_dest(a_dest), m_packet_size(a_packet_size) {}

    void Flow::schedule_packet_generation(std::uint32_t time) {
        auto generate_event_ptr = std::make_unique<Generate>(time, weak_from_this());
        Scheduler::get_instance().add(std::move(generate_event_ptr));
    }

void Flow::generate_packet() {
    if (auto src = m_src.lock()) {
        sim::Packet packet(PacketType::DATA, m_packet_size, weak_from_this());
        src->enqueue_packet(packet);
    }
}

std::shared_ptr<ISender> Flow::get_source() const noexcept { 
    return m_src.lock(); 
}

std::shared_ptr<IReceiver> Flow::get_destination() const noexcept { 
    return m_dest.lock(); 
}

}  // namespace sim