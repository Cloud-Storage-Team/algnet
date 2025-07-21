#include "tcp_tahoe_cc.hpp"

#include <spdlog/fmt/fmt.h>

#include "scheduler.hpp"

namespace sim {
TcpTahoeCC::TcpTahoeCC(Time a_dealay_threshold, double a_sstresh)
    : m_delay_threshold(a_dealay_threshold),
      m_ssthresh(a_sstresh),
      m_cwnd(1.0),
      m_last_congestion_detected(0),
      m_avg_rtt(0.0) {}

bool TcpTahoeCC::on_ack(Time rtt, bool ecn_flag) {
    update_avg_rtt(rtt);
    Time current_time = Scheduler::get_instance().get_current_time();
    if (ecn_flag || rtt >= m_delay_threshold) {
        // trigger_congestion
        if (current_time > m_last_congestion_detected + m_avg_rtt) {
            // To avoid too frequent cwnd and sstresh decreases
            m_last_congestion_detected = current_time;
            m_ssthresh = m_cwnd / 2;
            m_cwnd = 1.;
        }
        return true;
    }
    if (m_cwnd < m_ssthresh) {
        // Slow start
        m_cwnd++;
    } else {
        // Congestion avoidance
        m_cwnd += 1 / m_cwnd;
    }
    return false;
}

Time TcpTahoeCC::get_pacing_delay() const { return 0; }

double TcpTahoeCC::get_cwnd() const { return m_cwnd; }

std::string TcpTahoeCC::to_string() const {
    return fmt::format("[delay threshold: {}, cwnd: {}, sstresh: {}]",
                       m_delay_threshold, m_cwnd, m_ssthresh);
}

void TcpTahoeCC::update_avg_rtt(Time rtt) {
    if (m_avg_rtt == 0.0) {
        // If not initialized before
        m_avg_rtt = rtt;
    } else {
        m_avg_rtt = m_avg_rtt * M_RTT_WEIGHT_DECAY_FACTOR +
                    rtt * (1 - M_RTT_WEIGHT_DECAY_FACTOR);
    }
}

}  // namespace sim
