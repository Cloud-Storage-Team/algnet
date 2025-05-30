#include "tcp_aimd_flow.hpp"

#include <spdlog/fmt/fmt.h>

#include <algorithm>
#include <sstream>

#include "device/sender.hpp"
#include "event.hpp"
#include "logger/logger.hpp"
#include "metrics_collector.hpp"
#include "scheduler.hpp"

namespace sim {

TcpAimdFlow::TcpAimdFlow(Id a_id, std::shared_ptr<ISender> a_src,
                         std::shared_ptr<IReceiver> a_dest, Size a_packet_size,
                         Time a_delay_between_packets,
                         std::uint32_t a_packets_to_send,
                         Time a_delay_threshold, std::uint32_t a_ssthresh,
                         double a_beta)
    : m_src(a_src),
      m_dest(a_dest),
      m_packet_size(a_packet_size),
      m_delay_between_packets(a_delay_between_packets),
      m_packets_to_send(a_packets_to_send),
      m_delay_threshold(a_delay_threshold),
      m_ssthresh(a_ssthresh),
      m_cwnd(1),
      m_packets_in_flight(0),
      m_packets_acked(0),
      m_beta(a_beta),
      m_id(a_id) {
    LOG_INFO(to_string());
}

void TcpAimdFlow::start() {
    Time curr_time = Scheduler::get_instance().get_current_time();
    std::unique_ptr<Event> generate_event = std::make_unique<Generate>(
        curr_time, shared_from_this(), m_packet_size);
    Scheduler::get_instance().add(std::move(generate_event));

    std::unique_ptr<Event> metrics_event =
        std::make_unique<TcpMetric>(curr_time, shared_from_this());
    Scheduler::get_instance().add(std::move(metrics_event));
}

Time TcpAimdFlow::create_new_data_packet() {
    if (m_packets_to_send == 0) {
        return 0;
    }
    if (try_to_put_data_to_device()) {
        --m_packets_to_send;
    }
    return m_delay_between_packets;
}

void TcpAimdFlow::update(Packet packet, DeviceType type) {
    if (type != DeviceType::SENDER || packet.type != PacketType::ACK) {
        return;
    }

    Time current_time = Scheduler::get_instance().get_current_time();
    if (current_time < packet.send_time) {
        LOG_ERROR("Packet " + packet.to_string() +
                  " sending time less than current time; ignored");
        return;
    }

    Time delay = current_time - packet.send_time;
    LOG_INFO(fmt::format("Packet {} got in flow; delay = {}",
                         packet.to_string(), delay));

    MetricsCollector::get_instance().add_RTT(packet.flow->get_id(),
                                             current_time, delay);

    if (delay < m_delay_threshold) {
        if (m_packets_in_flight > 0) {
            m_packets_in_flight--;
        }
        m_packets_acked++;

        // Additive Increase
        m_cwnd += 1;
        try_to_put_data_to_device();
    } else {
        // Multiplicative Decrease
        m_ssthresh = std::max(1u, static_cast<std::uint32_t>(m_cwnd * m_beta));
        m_cwnd = m_ssthresh;
        m_packets_in_flight = 0;
    }

    LOG_INFO("New flow state: " + to_string());
}

std::shared_ptr<ISender> TcpAimdFlow::get_sender() const {
    return m_src.lock();
}
std::shared_ptr<IReceiver> TcpAimdFlow::get_receiver() const {
    return m_dest.lock();
}
Id TcpAimdFlow::get_id() const { return m_id; }

std::uint32_t TcpAimdFlow::get_cwnd() const { return m_cwnd; }

std::string TcpAimdFlow::to_string() const {
    std::ostringstream oss;
    oss << "TcpFlowAIMD[Id: " << m_id << ", packet size: " << m_packet_size
        << ", to send: " << m_packets_to_send << ", cwnd: " << m_cwnd
        << ", ssthresh: " << m_ssthresh << ", beta: " << m_beta << "]";
    return oss.str();
}

Packet TcpAimdFlow::generate_packet() {
    return Packet(PacketType::DATA, m_packet_size, this,
                  Scheduler::get_instance().get_current_time());
}

bool TcpAimdFlow::try_to_put_data_to_device() {
    if (m_packets_in_flight < m_cwnd) {
        m_packets_in_flight++;
        Packet packet = generate_packet();
        m_src.lock()->enqueue_packet(packet);
        return true;
    }
    return false;
}
}  // namespace sim
