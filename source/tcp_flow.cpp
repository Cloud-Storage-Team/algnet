#include "tcp_flow.hpp"
#include "event.hpp"
#include "packet.hpp"
#include "scheduler.hpp"
#include "sender.hpp"

namespace sim {

    TCPFlow::TCPFlow(ISender *a_src, IReceiver *a_dest, std::uint32_t a_packet_size,
           std::uint32_t a_delay_between_packets, std::uint32_t a_total_packets,
           std::uint32_t a_delay_threshold)
    : Flow(a_src, a_dest, a_packet_size),
      m_delay_between_packets(a_delay_between_packets),
      m_total_packets(a_total_packets),
      m_packets_sent(0),
      m_cwnd(1),
      m_ssthresh(128),
      m_linear_coeficient(2),
      m_packets_in_flight(0),
      m_packets_acked(0),
      m_last_send_time(0),
      m_delay_threshold(a_delay_threshold) {}

void TCPFlow::start(std::uint32_t time) {
    schedule_packet_generation(time);
}

bool TCPFlow::try_to_generate(std::uint32_t a_current_time) {
    if (m_packets_sent >= m_total_packets) {  // stop generate packets
        return false;
    }
    if (m_packets_in_flight < m_cwnd) {
        m_packets_in_flight++;
        m_packets_sent++;
        generate_packet();
        schedule_packet_generation(a_current_time + m_delay_between_packets);
        return true;
    }
    schedule_packet_generation(a_current_time + m_delay_between_packets);
    return false;
}

void TCPFlow::update(std::uint32_t delay) {
    if (delay < m_delay_threshold) {  // ack
        m_packets_in_flight--;
        if (m_cwnd < m_ssthresh) {
            m_cwnd++;
        } else {
            m_packets_acked++;
            if (m_packets_acked >= m_cwnd / m_linear_coeficient) {
                m_cwnd++;
                m_packets_acked = 0;
            }
        }
    } else {  // trigger_congestion
        m_ssthresh = m_cwnd / 2;
        m_cwnd = 1;
        m_packets_in_flight = 0;
    }
}

}  // namespace sim