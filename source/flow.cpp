#include "flow.hpp"

#include <memory>

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
      m_packets_to_send(a_packets_to_send) {}

void Flow::schedule_packet_generation(Time time) {
    auto generate_event_ptr =
        std::make_unique<Generate>(time, shared_from_this(), m_packet_size);
    Scheduler::get_instance().add(std::move(generate_event_ptr));
}

Packet Flow::generate_packet() {
    sim::Packet packet;
    packet.type = sim::PacketType::DATA;
    packet.size = m_packet_size;
    packet.flow = this;
    return packet;
}

void Flow::start(std::uint32_t time) { schedule_packet_generation(time); }

void Flow::update(Packet packet, DeviceType type) { ++m_updates_number; }

std::uint32_t Flow::get_updates_number() const { return m_updates_number; }

Time Flow::create_new_data_packet() {
    if (m_packets_to_send == 0) {
        return 0;
    }
    --m_packets_to_send;
    Packet data = generate_packet();
    m_flow_data.push(data);
    return add_data_to_device();
}

Time Flow::add_data_to_device() {
    m_src->enqueue_packet(m_flow_data.front());
    m_flow_data.pop();
    return m_delay_between_packets;
}   

std::shared_ptr<ISender> Flow::get_sender() const { return m_src; }

std::shared_ptr<IReceiver> Flow::get_receiver() const { return m_dest; }

}  // namespace sim
