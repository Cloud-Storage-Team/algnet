#include "flow.hpp"
#include "event.hpp"
#include "scheduler.hpp"
#include "packet.hpp"
#include "sender.hpp"
#include <iostream>

namespace sim {

    Flow::Flow(Device* src, size_t packet_size, size_t delay_between_packets, size_t total_packets)
        : m_src(src), 
          m_packet_size(packet_size), 
          m_delay_between_packets(delay_between_packets), 
          m_total_packets(total_packets),
          m_packets_sent(0),
          m_cwnd(1), 
          m_ssthresh(128), 
          m_linear_coeficient(1),
          m_packets_in_flight(0), 
          m_packets_acked(0),
          m_last_send_time(0) {}

    void Flow::start(size_t time){
        schedule_packet_generation(time);
    };

    bool Flow::try_to_generate(size_t current_time) { 
        if (m_packets_in_flight < m_cwnd && m_packets_sent<m_total_packets) { //maybe good to stop generating events if all packets are sent
            schedule_packet_generation(std::max(m_last_send_time + m_delay_between_packets, current_time));
            return true;
        }
        return false;
    }

    void Flow::update(size_t delay){
        if (delay < 100){
            ack();
        } else {
            trigger_congestion();
        }
    }

    void Flow::ack() {
        m_packets_in_flight--;
        if (m_cwnd < m_ssthresh){
            m_cwnd++;
        } else {
            m_packets_acked++;
            if (m_linear_coeficient * m_packets_acked >=  m_cwnd) {
                m_cwnd ++; 
                m_packets_acked = 0;
            }
        }
    }

    void Flow::trigger_congestion() {
        m_ssthresh = m_cwnd / 2; 
        m_cwnd = 1;
    }

    void Flow::schedule_packet_generation(int time) {
        Generate generate_event(this, m_packet_size);
        generate_event.time = time;
        Scheduler::get_instance().add(generate_event);
        ++m_packets_in_flight;
        ++m_packets_sent;
        m_last_send_time = time;
    }

}  // namespace sim