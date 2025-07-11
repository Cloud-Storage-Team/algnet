#include "basic_flow.hpp"

#include <spdlog/fmt/fmt.h>

#include "event/generate.hpp"
#include "logger/logger.hpp"
#include "metrics/metrics_collector.hpp"
#include "scheduler.hpp"

namespace sim {
std::string BasicFlow::packet_type_label = "type";
FlagManager<std::string, PacketFlagsBase> BasicFlow::m_flag_manager;
bool BasicFlow::m_is_flag_manager_initialized = false;

BasicFlow::BasicFlow(FlowCommon a_flow_common)
    : m_flow_common(std::move(a_flow_common)) {
    if (m_flow_common.src.lock() == nullptr) {
        throw std::invalid_argument("Sender for Flow is nullptr");
    }
    if (m_flow_common.dest.lock() == nullptr) {
        throw std::invalid_argument("Receiver for Flow is nullptr");
    }
    initialize_flag_manager();
}

void BasicFlow::start() {
    Time curr_time = Scheduler::get_instance().get_current_time();
    Scheduler::get_instance().add<Generate>(curr_time, shared_from_this(),
                                            m_flow_common.packet_size);
}

Time BasicFlow::create_new_data_packet() {
    if (m_flow_common.packets_to_send == 0) {
        return 0;
    }
    put_data_to_device();
    return m_flow_common.delay_between_packets;
}

void BasicFlow::update(Packet packet, DeviceType type) {
    (void)type;
    if (packet.dest_id == m_flow_common.dest.lock()->get_id() &&
        m_flag_manager.get_flag(packet, packet_type_label) ==
            PacketType::DATA) {
        // data packet arrived to destination device, send ack
        Packet ack(1, this, m_flow_common.dest.lock()->get_id(),
                   m_flow_common.src.lock()->get_id(), packet.sent_time,
                   packet.sent_bytes_at_origin, packet.ecn_capable_transport,
                   packet.congestion_experienced);

        m_flag_manager.set_flag(ack, packet_type_label, PacketType::ACK);
        m_flow_common.dest.lock()->enqueue_packet(ack);
    } else if (packet.dest_id == m_flow_common.src.lock()->get_id() &&
               m_flag_manager.get_flag(packet, packet_type_label) ==
                   PacketType::ACK) {
        // ask arrived to source device, calsulate metrics
        Time current_time = Scheduler::get_instance().get_current_time();

        double rtt = current_time - packet.sent_time;
        MetricsCollector::get_instance().add_RTT(packet.flow->get_id(),
                                                 current_time, rtt);

        double delivery_bit_rate =
            8 * (m_flow_common.sent_bytes - packet.sent_bytes_at_origin) / rtt;
        MetricsCollector::get_instance().add_delivery_rate(
            packet.flow->get_id(), current_time, delivery_bit_rate);

        ++m_flow_common.packets_acked;
    } else {
        LOG_ERROR(
            fmt::format("Called update on flow {} with some foreign packet {}",
                        get_id(), packet.to_string()));
    }
}

std::shared_ptr<IHost> BasicFlow::get_sender() const {
    return m_flow_common.src.lock();
}

std::shared_ptr<IHost> BasicFlow::get_receiver() const {
    return m_flow_common.dest.lock();
}

Id BasicFlow::get_id() const { return m_flow_common.id; }

std::uint32_t BasicFlow::get_updates_number() const {
    return m_flow_common.packets_acked;
}

void BasicFlow::initialize_flag_manager() {
    if (!m_is_flag_manager_initialized) {
        m_flag_manager.register_flag_by_amount(packet_type_label,
                                               PacketType::ENUM_SIZE);
        m_is_flag_manager_initialized = true;
    }
}

Packet BasicFlow::generate_packet() {
    sim::Packet packet(m_flow_common.generate_packet_header(), this);
    m_flag_manager.set_flag(packet, packet_type_label, PacketType::DATA);
    return packet;
}

void BasicFlow::put_data_to_device() {
    Packet packet = generate_packet();
    m_flow_common.src.lock()->enqueue_packet(packet);
    --m_flow_common.packets_to_send;
    m_flow_common.sent_bytes += packet.size;
}

}  // namespace sim
