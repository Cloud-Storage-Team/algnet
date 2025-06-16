#include "simple_flow.hpp"

#include <spdlog/fmt/fmt.h>

#include "event.hpp"
#include "logger/logger.hpp"
#include "metrics/metrics_collector.hpp"
#include "scheduler.hpp"

namespace sim {

SimpleFlow::SimpleFlow(Id a_id, std::shared_ptr<IHost> a_src,
                       std::shared_ptr<IHost> a_dest, Size a_packet_size,
                       Time a_delay_between_packets,
                       std::uint32_t a_packets_to_send)
    : m_id(a_id),
      m_src(a_src),
      m_dest(a_dest),
      m_packet_size(a_packet_size),
      m_delay_between_packets(a_delay_between_packets),
      m_updates_number(0),
      m_packets_to_send(a_packets_to_send) {
    if (m_src.lock() == nullptr) {
        throw std::invalid_argument("Sender for Flow is nullptr");
    }
    if (m_dest.lock() == nullptr) {
        throw std::invalid_argument("Receiver for Flow is nullptr");
    }
}

void SimpleFlow::start() {
    schedule_packet_generation(Scheduler::get_instance().get_current_time());
}

Packet SimpleFlow::generate_packet() {
    sim::Packet packet;
    packet.type = sim::PacketType::DATA;
    packet.size_byte = m_packet_size;
    packet.flow = this;
    packet.source_id = get_sender()->get_id();
    packet.dest_id = get_receiver()->get_id();
    packet.sent_time = Scheduler::get_instance().get_current_time();
    return packet;
}

void SimpleFlow::update(Packet packet, DeviceType type) {
    (void)type;
    if (packet.dest_id == m_dest.lock()->get_id() &&
        packet.type == PacketType::DATA) {
        // packet arrived to destination device, send ack
        Packet ack(PacketType::ACK, 1, this, m_dest.lock()->get_id(),
                   m_src.lock()->get_id(), packet.sent_time,
                   packet.sent_bytes_at_origin, packet.ecn_capable_transport,
                   packet.congestion_experienced);
        m_dest.lock()->enqueue_packet(ack);
    } else if (packet.dest_id == m_src.lock()->get_id() &&
               packet.type == PacketType::ACK) {
        ++m_updates_number;

        Time current_time = Scheduler::get_instance().get_current_time();
        MetricsCollector::get_instance().add_RTT(
            packet.flow->get_id(), current_time,
            current_time - packet.sent_time);
    } else {
        LOG_ERROR(
            fmt::format("Called update on flow {} with some foreign packet {}",
                        get_id(), packet.to_string()));
    }
}

std::uint32_t SimpleFlow::get_updates_number() const {
    return m_updates_number;
}

Time SimpleFlow::create_new_data_packet() {
    if (m_packets_to_send == 0) {
        return 0;
    }
    --m_packets_to_send;
    Packet data = generate_packet();
    m_sending_buffer.push(data);
    return put_data_to_device();
}

std::shared_ptr<IHost> SimpleFlow::get_sender() const { return m_src.lock(); }

std::shared_ptr<IHost> SimpleFlow::get_receiver() const {
    return m_dest.lock();
}

Id SimpleFlow::get_id() const { return m_id; }

Time SimpleFlow::put_data_to_device() {
    if (m_src.expired()) {
        LOG_ERROR("Flow source was deleted; can not put data to it");
        return 0;
    }
    m_sending_buffer.front().sent_time =
        Scheduler::get_instance().get_current_time();
    m_src.lock()->enqueue_packet(m_sending_buffer.front());
    m_sending_buffer.pop();
    return m_delay_between_packets;
}

void SimpleFlow::schedule_packet_generation(Time time) {
    auto generate_event_ptr =
        std::make_unique<Generate>(time, shared_from_this(), m_packet_size);
    Scheduler::get_instance().add(std::move(generate_event_ptr));
}

}  // namespace sim
