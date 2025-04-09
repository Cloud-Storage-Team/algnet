#include "flow.hpp"

#include "logger.hpp"
#include "receiver.hpp"
#include "scheduler.hpp"

namespace sim {

Flow::Flow(ISender* a_src, IReceiver* a_dest, Size a_packet_size,
           Time a_delay_between_packets)
    : m_src(a_src),
      m_dest(a_dest),
      m_packet_size(a_packet_size),
      m_delay_between_packets(a_delay_between_packets) {}

void Flow::schedule_packet_generation(Time time) {
    auto generate_event_ptr =
        std::make_unique<Generate>(time, shared_from_this(), m_packet_size);
    Scheduler::get_instance().add(std::move(generate_event_ptr));
}

void Flow::generate_packet() {
    sim::Packet packet;
    packet.type = sim::PacketType::DATA;
    packet.size = m_packet_size;
    packet.flow = this;
    m_src->enqueue_packet(packet);
}

void Flow::start(std::uint32_t time) { schedule_packet_generation(time); }

void Flow::update() {}

bool Flow::try_to_generate(Time a_current_time) {
    generate_packet();
    schedule_packet_generation(a_current_time + m_delay_between_packets);
    return true;
}

std::shared_ptr<ISender> Flow::get_sender() const {
    return nullptr;  // TODO: implement this
}

std::shared_ptr<IReceiver> Flow::get_receiver() const {
    return nullptr;  // TODO: implement this
}

}  // namespace sim
