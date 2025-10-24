#include "tcp_tahoe_cc.hpp"

#include <spdlog/fmt/fmt.h>

#include "scheduler.hpp"

namespace sim {
TcpTahoeCC::TcpTahoeCC(double a_start_cwnd, double a_sstresh)
    : m_ssthresh(a_sstresh),
      m_cwnd(a_start_cwnd),
      m_last_congestion_detected(0),
      m_last_avg_rtt(0) {}

void TcpTahoeCC::on_ack([[maybe_unused]] TimeNs rtt, TimeNs avg_rtt,
                        bool ecn_flag) {
    m_last_avg_rtt = avg_rtt;
    if (ecn_flag) {
        on_timeout();
    } else if (m_cwnd < m_ssthresh) {
        // Slow start
        m_cwnd++;
    } else {
        // Congestion avoidance
        m_cwnd += 1 / m_cwnd;
    }
}

void TcpTahoeCC::on_timeout() {
    TimeNs current_time = Scheduler::get_instance().get_current_time();
    if (current_time > m_last_congestion_detected + m_last_avg_rtt) {
        // To avoid too frequent cwnd and sstresh decreases
        m_last_congestion_detected = current_time;
        m_ssthresh = std::max(0.0, m_cwnd / 2);
        m_cwnd = 1.;
    }
}

TimeNs TcpTahoeCC::get_pacing_delay() const { return TimeNs(0); }

double TcpTahoeCC::get_cwnd() const { return m_cwnd; }

std::string TcpTahoeCC::to_string() const {
    return fmt::format("[cwnd: {}, ssthresh: {}]", m_cwnd, m_ssthresh);
}

}  // namespace sim
