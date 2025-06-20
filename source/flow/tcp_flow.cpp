#include "flow/tcp_flow.hpp"

#include <spdlog/fmt/fmt.h>

#include "event.hpp"
#include "logger/logger.hpp"
#include "metrics/metrics_collector.hpp"
#include "scheduler/scheduler.hpp"

namespace sim {

TcpFlow::TcpFlow(Id a_id, std::shared_ptr<ISender> a_src,
                 std::shared_ptr<IReceiver> a_dest, Size a_packet_size,
                 Time a_delay_between_packets, std::uint32_t a_packets_to_send,
                 Time a_delay_threshold, std::uint32_t a_ssthresh)
    : m_id(a_id),
      m_src(a_src),
      m_dest(a_dest),
      m_packet_size(a_packet_size),
      m_delay_between_packets(a_delay_between_packets),
      m_packets_to_send(a_packets_to_send),
      m_delay_threshold(a_delay_threshold),
      m_ssthresh(a_ssthresh),
      m_cwnd(1.),
      m_packets_in_flight(0),
      m_packets_acked(0),
      m_sent_bytes(0) {
    if (m_src.lock() == nullptr) {
        throw std::invalid_argument("Sender for TcpFlow is nullptr");
    }
    if (m_dest.lock() == nullptr) {
        throw std::invalid_argument("Receiver for TcpFlow is nullptr");
    }
}

void TcpFlow::start() {
    Time curr_time = Scheduler::get_instance().get_current_time();
    Scheduler::get_instance().add<Generate>(curr_time, shared_from_this(),
                                            m_packet_size);

    Scheduler::get_instance().add<TcpMetric>(curr_time, shared_from_this());
}

Time TcpFlow::create_new_data_packet() {
    if (m_packets_to_send == 0) {
        return 0;
    }
    if (try_to_put_data_to_device()) {
        --m_packets_to_send;
    }

    return m_delay_between_packets;
}

void TcpFlow::update(Packet packet, DeviceType type) {
    if (type != DeviceType::SENDER || packet.type != PacketType::ACK) {
        return;
    }

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
        8 * (m_sent_bytes - packet.sent_bytes_at_origin) / rtt;
    MetricsCollector::get_instance().add_delivery_rate(
        packet.flow->get_id(), current_time, delivery_bit_rate);

    if (rtt < m_delay_threshold) {
        if (m_packets_in_flight > 0) {
            m_packets_in_flight--;
        }
        m_packets_acked++;
        if (m_cwnd < m_ssthresh) {
            m_cwnd *= 2;
        } else {
            m_cwnd += 1.;
        }
        try_to_put_data_to_device();
    } else {  // trigger_congestion
        m_ssthresh = m_cwnd / 2;
        m_cwnd = 1.;
        m_packets_in_flight = 0;
    }

    LOG_INFO("New flow: " + to_string());
}

std::shared_ptr<ISender> TcpFlow::get_sender() const { return m_src.lock(); }

std::shared_ptr<IReceiver> TcpFlow::get_receiver() const {
    return m_dest.lock();
}

Id TcpFlow::get_id() const { return m_id; }

std::string TcpFlow::to_string() const {
    std::ostringstream oss;
    oss << "TcpFlow[";
    oss << "Id: " << m_id;
    oss << ", packet size: " << m_packet_size;
    oss << ", to send packets: " << m_packets_to_send;
    oss << ", delay: " << m_delay_between_packets;
    oss << ", delay threshhold: " << m_delay_threshold;
    oss << ", cwnd: " << m_cwnd;
    oss << ", packets_in_flight: " << m_packets_in_flight;
    oss << ", acked packets: " << m_packets_acked;
    oss << "]";
    return oss.str();
}

Packet TcpFlow::generate_packet() {
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

bool TcpFlow::try_to_put_data_to_device() {
    if (m_packets_in_flight < m_cwnd) {
        m_packets_in_flight++;
        Packet packet = generate_packet();
        m_sent_bytes += packet.size_byte;
        m_src.lock()->enqueue_packet(packet);
        return true;
    }
    return false;
}

double TcpFlow::get_cwnd() const { return m_cwnd; }

}  // namespace sim
