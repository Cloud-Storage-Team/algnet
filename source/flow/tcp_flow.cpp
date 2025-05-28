#include "tcp_flow.hpp"

#include <spdlog/fmt/fmt.h>

#include <sstream>

#include "event.hpp"
#include "iostream"
#include "scheduler.hpp"
<<<<<<< HEAD
#include "logger/logger.hpp"
=======
>>>>>>> origin/main
#include "utils/identifier_factory.hpp"
#include "metrics_collector.hpp"

namespace sim {

TcpFlow::TcpFlow(std::shared_ptr<ISender> a_src,
                 std::shared_ptr<IReceiver> a_dest, Size a_packet_size,
                 Time a_delay_between_packets, std::uint32_t a_packets_to_send,
                 Time a_delay_threshold, std::uint32_t a_ssthresh)
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
      m_id(IdentifierFactory::get_instance().generate_id()) {
    LOG_INFO(to_string());
}

TcpFlow::~TcpFlow() { LOG_INFO(to_string()); }

void TcpFlow::start() {
    auto generate_event = std::make_unique<Generate>(Scheduler::get_instance().get_current_time(),
                            shared_from_this(), m_packet_size);
    Scheduler::get_instance().add(std::move(generate_event));
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
    if (current_time < packet.send_time) {
        LOG_ERROR("Packet " + packet.to_string() +
                  " sending time less that current time; ignored");
        return;
    }

    Time delay = current_time - packet.send_time;

    LOG_INFO(fmt::format("Packet {} got in flow; delay = {}",
                         packet.to_string(), to_string(), delay));

    MetricsCollector::get_instance().add_RTT(packet.flow->get_id(), delay);

    if (delay < m_delay_threshold) {  // ask
        if (m_packets_in_flight > 0) {
            m_packets_in_flight--;
        }
        m_packets_acked++;
        if (m_cwnd < m_ssthresh) {
            m_cwnd *= 2;
        } else {
            m_cwnd++;
        }
        try_to_put_data_to_device();
    } else {  // trigger_congestion
        m_ssthresh = m_cwnd / 2;
        m_cwnd = 1;
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
    oss << ", to send packages: " << m_packets_to_send;
    oss << ", delay: " << m_delay_between_packets;
    oss << ", delay threshhold: " << m_delay_threshold;
    oss << ", cwnd: " << m_cwnd;
    oss << ", packets_in_flight: " << m_packets_in_flight;
    oss << ", asked packges: " << m_packets_acked;
    oss << "]";
    return oss.str();
}

Packet TcpFlow::generate_packet() {
    return Packet(PacketType::DATA, m_packet_size, this,
                  Scheduler::get_instance().get_current_time());
}

bool TcpFlow::try_to_put_data_to_device() {
    if (m_packets_in_flight < m_cwnd) {
        m_packets_in_flight++;
        Packet packet = generate_packet();
        m_src.lock()->enqueue_packet(packet);
        return true;
    }
    return false;
}

}  // namespace sim
