#include "tcp_tahoe_cc.hpp"

#include <spdlog/fmt/fmt.h>

#include "scheduler.hpp"

namespace sim {
TcpTahoeCC::TcpTahoeCC(TimeNs a_delay_threshold, double a_sstresh)
    : m_delay_threshold(a_delay_threshold),
      m_ssthresh(a_sstresh),
      m_cwnd(1.0),
      m_last_congestion_detected(0) {}

bool TcpTahoeCC::on_ack([[maybe_unused]] TimeNs rtt, TimeNs avg_rtt,
                        bool ecn_flag) {
    Time current_time = Scheduler::get_instance().get_current_time();
    if (ecn_flag || avg_rtt > m_delay_threshold) {
        // trigger_congestion
        if (current_time > m_last_congestion_detected + avg_rtt) {
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

TimeNs TcpTahoeCC::get_pacing_delay() const { return TimeNs(0); }

double TcpTahoeCC::get_cwnd() const { return m_cwnd; }

std::string TcpTahoeCC::to_string() const {
    return fmt::format("[delay threshold: {}, cwnd: {}, ssthresh: {}]",
                       m_delay_threshold.value(), m_cwnd, m_ssthresh);
}

}  // namespace sim
