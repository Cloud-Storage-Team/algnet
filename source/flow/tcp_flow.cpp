#include "flow/tcp_flow.hpp"

#include <spdlog/fmt/fmt.h>

#include "event/generate.hpp"
#include "logger/logger.hpp"
#include "metrics/metrics_collector.hpp"
#include "scheduler.hpp"

namespace sim {
std::string TcpFlow::packet_type_label = "type";
FlagManager<std::string, PacketFlagsBase> TcpFlow::m_flag_manager;
bool TcpFlow::m_is_flag_manager_initialized = false;

TcpFlow::TcpFlow(FlowCommon a_flow_common, std::uint32_t a_ssthresh,
                 bool a_ecn_capable)
    : m_flow_common(std::move(a_flow_common)),
      m_ssthresh(a_ssthresh),
      m_cwnd(1.),
      m_ecn_capable(a_ecn_capable) {
    if (m_flow_common.src.lock() == nullptr) {
        throw std::invalid_argument("Sender for TcpFlow is nullptr");
    }
    if (m_flow_common.dest.lock() == nullptr) {
        throw std::invalid_argument("Receiver for TcpFlow is nullptr");
    }
    initialize_flag_manager();
}

void TcpFlow::start() {
    Time curr_time = Scheduler::get_instance().get_current_time();
    Scheduler::get_instance().add<Generate>(curr_time, shared_from_this(),
                                            m_flow_common.packet_size);
}

Time TcpFlow::create_new_data_packet() {
    if (m_flow_common.packets_to_send == 0) {
        return 0;
    }
    try_to_put_data_to_device();
    return m_flow_common.delay_between_packets;
}

void TcpFlow::update(Packet packet, DeviceType type) {
    (void)type;
    if (packet.dest_id == m_flow_common.dest.lock()->get_id() &&
        m_flag_manager.get_flag(packet, packet_type_label) ==
            PacketType::DATA) {
        // data packet arrived to destination device; send ack
        Packet ack(1, this, m_flow_common.dest.lock()->get_id(),
                   m_flow_common.src.lock()->get_id(), packet.sent_time,
                   packet.sent_bytes_at_origin, packet.ecn_capable_transport,
                   packet.congestion_experienced);

        m_flag_manager.set_flag(packet, packet_type_label, PacketType::ACK);
        m_flow_common.dest.lock()->enqueue_packet(ack);
    } else if (packet.dest_id == m_flow_common.src.lock()->get_id() &&
               m_flag_manager.get_flag(packet, packet_type_label) ==
                   PacketType::ACK) {
        // ACK delivered to soiurce device; calculate metrics, update internal
        // state
        Time current_time = Scheduler::get_instance().get_current_time();
        if (current_time < packet.sent_time) {
            LOG_ERROR("Packet " + packet.to_string() +
                      " sending time less that current time; ignored");
            return;
        }

        Time rtt = current_time - packet.sent_time;
        MetricsCollector::get_instance().add_RTT(packet.flow->get_id(),
                                                 current_time, rtt);

        double delivery_bit_rate =
            8 * (m_flow_common.sent_bytes - packet.sent_bytes_at_origin) / rtt;
        MetricsCollector::get_instance().add_delivery_rate(
            packet.flow->get_id(), current_time, delivery_bit_rate);

        double old_cwnd = m_cwnd;

        if (rtt >= m_flow_common.delay_threshold ||
            packet.congestion_experienced) {
            // trigger_congestion
            m_ssthresh = m_cwnd / 2;
            m_cwnd = 1.;
            m_flow_common.packets_in_flight = 0;
        } else {
            if (m_flow_common.packets_in_flight > 0) {
                m_flow_common.packets_in_flight--;
            }
            m_flow_common.packets_acked++;
            if (m_cwnd < m_ssthresh) {
                m_cwnd *= 2;
            } else {
                m_cwnd += 1.;
            }
            try_to_put_data_to_device();
        }
        if (old_cwnd != m_cwnd) {
            MetricsCollector::get_instance().add_cwnd(
                m_flow_common.id, current_time - 1, old_cwnd);
            MetricsCollector::get_instance().add_cwnd(m_flow_common.id,
                                                      current_time, m_cwnd);
        }
    } else {
        LOG_ERROR(
            fmt::format("Called update on flow {} with some foreign packet {}",
                        get_id(), packet.to_string()));
    }
}

std::shared_ptr<IHost> TcpFlow::get_sender() const {
    return m_flow_common.src.lock();
}

std::shared_ptr<IHost> TcpFlow::get_receiver() const {
    return m_flow_common.dest.lock();
}

Id TcpFlow::get_id() const { return m_flow_common.id; }

void TcpFlow::initialize_flag_manager() {
    if (!m_is_flag_manager_initialized) {
        m_flag_manager.register_flag_by_amount(packet_type_label,
                                               PacketType::ENUM_SIZE);
        m_is_flag_manager_initialized = true;
    }
}

Packet TcpFlow::generate_packet() {
    sim::Packet packet(m_flow_common.generate_packet_header(), this);
    m_flag_manager.set_flag(packet, packet_type_label, PacketType::DATA);
    return packet;
}

bool TcpFlow::try_to_put_data_to_device() {
    if (m_flow_common.packets_in_flight >= m_cwnd) {
        return false;
    }
    Packet packet = generate_packet();
    m_flow_common.src.lock()->enqueue_packet(packet);
    --m_flow_common.packets_to_send;
    m_flow_common.sent_bytes += packet.size;
    m_flow_common.packets_in_flight++;
    return true;
}

}  // namespace sim
