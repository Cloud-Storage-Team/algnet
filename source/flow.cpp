#include "flow.hpp"

#include <iostream>

#include "event.hpp"
#include "packet.hpp"
#include "scheduler.hpp"
#include "sender.hpp"

namespace sim {

Flow::Flow(Device* a_src, Device* a_dest, std::uint32_t a_packet_size,
           std::uint32_t a_delay_between_packets, std::uint32_t a_total_packets,
           std::uint32_t a_delay_threshold)
    : m_src(a_src),
      m_dest(a_dest),
      m_packet_size(a_packet_size),
      m_delay_between_packets(a_delay_between_packets),
      m_total_packets(a_total_packets),
      m_packets_sent(0),
      m_cwnd(1),
      m_ssthresh(128),
      m_linear_coeficient(1),
      m_packets_in_flight(0),
      m_packets_acked(0),
      m_last_send_time(0),
      m_delay_threshold(a_delay_threshold) {}

void Flow::start(std::uint32_t time) {
    Generate generate_event(this);
    generate_event.time = time;
    Scheduler::get_instance().add(generate_event);
}

bool Flow::try_to_generate(std::uint32_t a_current_time) {
    if (m_packets_sent >= m_total_packets) {  // stop generate packets
        return false;
    }
    if (m_packets_in_flight < m_cwnd) {
        m_packets_in_flight++;
        m_packets_sent++;
        Packet p(PacketType::DATA, m_packet_size, this);  // TODO constructor
        m_src.add_packet(p);  // TODO add_packet method
        schedule_packet_generation(a_current_time + m_delay_between_packets);
        return true;
    }
    schedule_packet_generation(a_current_time + m_delay_between_packets);
    return false;
}

void Flow::update(std::uint32_t delay) {
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

void Flow::schedule_packet_generation(std::uint32_t time) {
    Generate generate_event(this);
    generate_event.time = time;
    Scheduler::get_instance().add(generate_event);
}

}  // namespace sim