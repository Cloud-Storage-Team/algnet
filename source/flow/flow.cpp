#include "flow/flow.hpp"

#include "event.hpp"
#include "logger/logger.hpp"
#include "metrics/metrics_collector.hpp"
#include "scheduler/scheduler.hpp"

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
      m_packets_to_send(a_packets_to_send),
      m_sent_bytes(0) {
    if (m_src.lock() == nullptr) {
        throw std::invalid_argument("Sender for Flow is nullptr");
    }
    if (m_dest.lock() == nullptr) {
        throw std::invalid_argument("Receiver for Flow is nullptr");
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
    packet.sent_time = Scheduler::get_instance().get_current_time();
    packet.sent_bytes_at_origin = m_sent_bytes;
    return packet;
}

void Flow::update(Packet packet, DeviceType type) {
    if (packet.type != PacketType::ACK || type != DeviceType::SENDER) {
        return;
    }
    m_updates_number++;

    Time current_time = Scheduler::get_instance().get_current_time();

    double rtt = current_time - packet.sent_time;
    MetricsCollector::get_instance().add_RTT(packet.flow->get_id(),
                                             current_time, rtt);

    double delivery_bit_rate =
        8 * (m_sent_bytes - packet.sent_bytes_at_origin) / rtt;
    MetricsCollector::get_instance().add_delivery_rate(
        packet.flow->get_id(), current_time, delivery_bit_rate);
}

std::uint32_t Flow::get_updates_number() const { return m_updates_number; }

Time Flow::create_new_data_packet() {
    if (m_packets_to_send == 0) {
        return 0;
    }
    --m_packets_to_send;
    Packet packet = generate_packet();
    // Note: sent_time and m_sent_bytes are evaluated at time of pushing
    // the packet to the m_sending_buffer
    m_sent_bytes += packet.size_byte;
    m_sending_buffer.push(packet);
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
    Packet packet = m_sending_buffer.front();
    m_src.lock()->enqueue_packet(packet);
    m_sending_buffer.pop();
    return m_delay_between_packets;
}

void Flow::schedule_packet_generation(Time time) {
    Scheduler::get_instance().add<Generate>(time, shared_from_this(),
                                            m_packet_size);
}

}  // namespace sim
