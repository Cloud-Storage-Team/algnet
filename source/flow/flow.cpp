#include "flow.hpp"

#include <memory>
#include <string>

#include "device/receiver.hpp"
#include "device/sender.hpp"
#include "logger/logger.hpp"
#include "metrics/metrics_collector.hpp"
#include "packet.hpp"
#include "scheduler.hpp"

namespace sim {

Flow::Flow(Id a_id, std::shared_ptr<ISender> a_src,
           std::shared_ptr<IReceiver> a_dest, Size a_packet_size,
           Time a_delay_between_packets, std::uint32_t a_packets_to_send)
    : m_id(a_id),
      m_src(a_src),
      m_dest(a_dest),
      m_packet_size(a_packet_size),
      m_delay_between_packets(a_delay_between_packets),
      m_updates_number(0),
      m_packets_to_send(a_packets_to_send) {
    if (m_src.lock() == nullptr) {
        throw std::runtime_error("Sender for TcpFlow is nullptr");
    }
    if (m_dest.lock() == nullptr) {
        throw std::runtime_error("Receiver for TcpFlow is nullptr");
    }
}

void Flow::start() {
    schedule_packet_generation(Scheduler::get_instance().get_current_time());
}

Packet Flow::generate_packet() {
    sim::Packet packet;
    packet.type = sim::PacketType::DATA;
    packet.size_byte = m_packet_size;
    packet.flow = this;
    packet.source_id = get_sender()->get_id();
    packet.dest_id = get_receiver()->get_id();
    packet.RTT = 0;
    packet.send_time = Scheduler::get_instance().get_current_time();
    return packet;
}

void Flow::update(Packet packet, DeviceType type) {
    if (packet.type != PacketType::ACK || type != DeviceType::SENDER) {
        return;
    }
    ++m_updates_number;

    Time current_time = Scheduler::get_instance().get_current_time();
    MetricsCollector::get_instance().add_RTT(
        packet.flow->get_id(),
        current_time,
        packet.RTT + current_time - packet.send_time);
}

std::uint32_t Flow::get_updates_number() const { return m_updates_number; }

Time Flow::create_new_data_packet() {
    if (m_packets_to_send == 0) {
        return 0;
    }
    --m_packets_to_send;
    Packet data = generate_packet();
    m_sending_buffer.push(data);
    return put_data_to_device();
}

std::shared_ptr<ISender> Flow::get_sender() const { return m_src.lock(); }

std::shared_ptr<IReceiver> Flow::get_receiver() const { return m_dest.lock(); }

Id Flow::get_id() const { return m_id; }

Time Flow::put_data_to_device() {
    if (m_src.expired()) {
        LOG_ERROR("Flow source was deleted; can not put data to it");
        return 0;
    }
    m_sending_buffer.front().send_time =
        Scheduler::get_instance().get_current_time();
    m_src.lock()->enqueue_packet(m_sending_buffer.front());
    m_sending_buffer.pop();
    return m_delay_between_packets;
}

void Flow::schedule_packet_generation(Time time) {
    auto generate_event_ptr =
        std::make_unique<Generate>(time, shared_from_this(), m_packet_size);
    Scheduler::get_instance().add(std::move(generate_event_ptr));
}

}  // namespace sim
