#include <memory>

#include "flow.hpp"
#include "device/receiver.hpp"
#include "scheduler.hpp"

namespace sim {

Flow::Flow(std::shared_ptr<ISender> a_src, std::shared_ptr<IReceiver> a_dest,
           Size a_packet_size, Time a_delay_between_packets,
           std::uint32_t a_packets_to_send)
    : m_src(a_src),
      m_dest(a_dest),
      m_packet_size(a_packet_size),
      m_delay_between_packets(a_delay_between_packets),
      m_updates_number(0),
      m_packets_to_send(a_packets_to_send),
      m_id(IdentifierFactory::get_instance().generate_id()) {}

void Flow::schedule_packet_generation(Time time) {
    auto generate_event_ptr =
        std::make_unique<Generate>(time, shared_from_this(), m_packet_size);
    Scheduler::get_instance().add(std::move(generate_event_ptr));
}

void Flow::generate_packet() {
    if (auto src = m_src.lock()) {
        sim::Packet packet(PacketType::DATA, m_packet_size, weak_from_this());
        src->enqueue_packet(packet);
    }
}

void Flow::start(Time time) { schedule_packet_generation(time); }

void Flow::update() { ++m_updates_number; }

std::uint32_t Flow::get_updates_number() const { return m_updates_number; }

Time Flow::try_to_generate() {
    if (m_packets_to_send == 0) {
        return 0;
    }
    --m_packets_to_send;
    generate_packet();
    return m_delay_between_packets;
}

std::shared_ptr<ISender> Flow::get_sender() const { return m_src.lock();  }

std::shared_ptr<IReceiver> Flow::get_receiver() const { return m_dest.lock(); }

Id Flow::get_id() const { return m_id; }

}  // namespace sim
