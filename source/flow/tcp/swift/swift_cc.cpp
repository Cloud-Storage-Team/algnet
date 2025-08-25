#include "swift_cc.hpp"

#include <spdlog/fmt/fmt.h>

#include <algorithm>
#include <cmath>
#include <string>

#include "scheduler.hpp"

namespace sim {

TcpSwiftCC::TcpSwiftCC(TimeNs a_base_target, double a_additive_inc,
                       double a_md_beta, double a_max_mdf, double a_fs_range,
                       double a_fs_min_cwnd, double a_fs_max_cwnd)
    : m_base_target(a_base_target),
      m_ai(a_additive_inc),
      m_beta_md(a_md_beta),
      m_max_mdf(a_max_mdf),
      m_fs_range_ns(a_base_target * a_fs_range),
      m_fs_min_cwnd(a_fs_min_cwnd),
      m_fs_max_cwnd(a_fs_max_cwnd),
      m_cwnd(1.0),
      m_last_decrease(Scheduler::get_instance().get_current_time()),
      m_last_rtt(a_base_target),
      m_retransmit_cnt(0) {
    // ---------- pre‑compute α and β for flow‑scaling ----------
    const double inv_sqrt_min = 1.0 / std::sqrt(m_fs_min_cwnd);
    const double inv_sqrt_max = 1.0 / std::sqrt(m_fs_max_cwnd);
    const double denom = inv_sqrt_min - inv_sqrt_max;
    // α chosen so that flow_term == FS_RANGE at cwnd = FS_MIN_CWND
    m_alpha_flow = m_fs_range_ns / denom;
    m_beta_flow = m_alpha_flow / std::sqrt(m_fs_max_cwnd);
}

void TcpSwiftCC::on_ack(TimeNs rtt, [[maybe_unused]] TimeNs avg_rtt,
                        [[maybe_unused]] bool ecn_flag) {
    if (rtt < TimeNs(1.0)) {
        throw std::runtime_error("Invalid rtt < 1");
    }
    m_retransmit_cnt = 0;
    m_last_rtt = rtt;

    // dynamic target_delay
    const TimeNs target_delay = compute_target_delay();

    const bool can_decrease = compute_can_decrease();
    double new_cwnd = m_cwnd;
    // ---------- AIMD ----------
    if (rtt < target_delay) {
        // Additive‑Increase
        if (new_cwnd > 1.0)
            new_cwnd += m_ai / new_cwnd;
        else
            new_cwnd += m_ai;
    } else if (can_decrease) {
        // Multiplicative‑Decrease based on «overshoot»
        const double overshoot = (rtt - target_delay) / rtt;
        new_cwnd *= std::max(1.0 - m_beta_md * overshoot, 1.0 - m_max_mdf);
    }
    update_cwnd(new_cwnd);
}

void TcpSwiftCC::on_timeout() {
    const bool can_decrease = compute_can_decrease();
    m_retransmit_cnt++;
    double new_cwnd = m_cwnd;
    if (m_retransmit_cnt >= M_RETX_RESET_THRESHOLD) {
        new_cwnd = M_MIN_CWND;
    } else if (can_decrease) {
        new_cwnd = (1 - m_max_mdf) * m_cwnd;
    }
    update_cwnd(new_cwnd);
}

TimeNs TcpSwiftCC::get_pacing_delay() const {
    if (m_cwnd > 1.0) {
        return TimeNs(0);
    }
    return m_last_rtt * (1.0 / m_cwnd - 1.0);
}

double TcpSwiftCC::get_cwnd() const { return m_cwnd; }

std::string TcpSwiftCC::to_string() const {
    return fmt::format("[target_now: {} ns, cwnd: {:0.3f}]",
                       compute_target_delay().value_nanoseconds(), m_cwnd);
}

TimeNs TcpSwiftCC::compute_target_delay() const {
    if (m_cwnd < m_fs_min_cwnd) {
        return m_base_target + m_fs_range_ns;
    } else if (m_cwnd < m_fs_max_cwnd) {
        TimeNs flow_term = (m_alpha_flow / std::sqrt(m_cwnd)) - m_beta_flow;
        return m_base_target + flow_term;
    } else {
        return m_base_target;
    }
}

bool TcpSwiftCC::compute_can_decrease() const {
    TimeNs current_time = Scheduler::get_instance().get_current_time();
    return (current_time - m_last_decrease) > m_last_rtt;
}

void TcpSwiftCC::update_cwnd(double new_cwnd) {
    new_cwnd = std::clamp(new_cwnd, M_MIN_CWND, M_MAX_CWND);
    if (new_cwnd < m_cwnd) {
        m_last_decrease = Scheduler::get_instance().get_current_time();
    }
    m_cwnd = new_cwnd;
}

}  // namespace sim
