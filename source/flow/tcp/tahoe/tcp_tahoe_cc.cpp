#include "tcp_tahoe_cc.hpp"

#include <spdlog/fmt/fmt.h>

namespace sim {
TcpTahoeCC::TcpTahoeCC(TimeNs a_dealay_threshold, double a_sstresh)
    : m_delay_threshold(a_dealay_threshold),
      m_ssthresh(a_sstresh),
      m_cwnd(1.0) {}

bool TcpTahoeCC::on_ack(TimeNs rtt, bool ecn_flag) {
    if (ecn_flag || rtt > m_delay_threshold) {
        // trigger_congestion
        m_ssthresh = m_cwnd / 2;
        m_cwnd = 1.;
        return true;
    }
    if (m_cwnd < m_ssthresh) {
        m_cwnd++;
    } else {
        m_cwnd += 1 / m_cwnd;
    }
    return false;
}

TimeNs TcpTahoeCC::get_pacing_delay() const { return TimeNs(0); }

double TcpTahoeCC::get_cwnd() const { return m_cwnd; }

std::string TcpTahoeCC::to_string() const {
    return fmt::format("[delay threshold: {}, cwnd: {}, sstresh: {}]",
                       m_delay_threshold.value(), m_cwnd, m_ssthresh);
}

}  // namespace sim