#include "swift_cc.hpp"

#include <spdlog/fmt/fmt.h>
#include <algorithm>
#include <cmath>
#include <string>

namespace sim {

TcpSwiftCC::TcpSwiftCC(TimeNs base_target,
                       double  a_additive_inc,
                       double  a_md_beta,
                       double  a_max_mdf,
                       double  fs_range,
                       double  fs_min_cwnd,
                       double  fs_max_cwnd)
    : m_base_target(base_target),
      m_ai(a_additive_inc),
      m_beta_md(a_md_beta),
      m_max_mdf(a_max_mdf),
      m_fs_range(fs_range),
      m_fs_min_cwnd(fs_min_cwnd),
      m_fs_max_cwnd(fs_max_cwnd),
      m_cwnd(1.0),
      m_last_decrease(Scheduler::get_instance().get_current_time()),
      m_last_rtt(base_target) {

    // ---------- pre‑compute α and β for flow‑scaling ----------
    const long double inv_sqrt_min = 1.0 / std::sqrt(m_fs_min_cwnd);
    const long double inv_sqrt_max = 1.0 / std::sqrt(m_fs_max_cwnd);
    const long double denom        = inv_sqrt_min - inv_sqrt_max;
    // α chosen so that flow_term == FS_RANGE at cwnd = FS_MIN_CWND
    m_alpha = m_fs_range / denom;
    m_beta_flow = -(m_alpha / std::sqrt(m_fs_max_cwnd));   // ≤ 0
}

TimeNs TcpSwiftCC::compute_target_delay() const {
    long double flow_term = (m_alpha / std::sqrt(m_cwnd)) + m_beta_flow;
    flow_term = std::clamp(flow_term, 0.0L, static_cast<long double>(m_fs_range));
    return m_base_target + TimeNs{flow_term}; // TO-DO: use m_base_target + TimeNs{flow_term} + h*hops_num
}

bool TcpSwiftCC::on_ack(TimeNs rtt_us,
                        [[maybe_unused]] TimeNs avg_rtt,
                        bool ecn_flag) {
    (void)ecn_flag;
    m_last_rtt = rtt_us;
    if (rtt_us < TimeNs(1.0L))
        throw std::runtime_error("Invalid rtt < 1");

    // dynamic delay_target
    const TimeNs delay_target = compute_target_delay();

    TimeNs now_tp       = Scheduler::get_instance().get_current_time();
    const bool can_mdf  = (now_tp - m_last_decrease) > rtt_us;
    bool md_performed   = false;

    // ---------- AIMD ----------
    if (rtt_us < delay_target) {
        // Additive‑Increase
        if (m_cwnd >= 1.0)
            m_cwnd += (m_ai / m_cwnd);
        else
            m_cwnd += m_ai;
    } else if (can_mdf) {
        // Multiplicative‑Decrease based on «overshoot»
        const long double overshoot = (rtt_us - delay_target) / rtt_us;
        const long double factor = std::max(1.0L - m_beta_md * overshoot, 1.0L - m_max_mdf);
        m_cwnd = std::max(MIN_CWND, static_cast<long double>(m_cwnd) * factor);
        m_last_decrease = now_tp;
        md_performed    = true;
    }

    m_cwnd = std::clamp(m_cwnd, static_cast<double>(MIN_CWND), MAX_CWND);
    return md_performed;
}

TimeNs TcpSwiftCC::get_pacing_delay() const {
    if (m_cwnd > 1.0) return TimeNs{0};
    long double gap_us = m_last_rtt.value_nanoseconds() / m_cwnd;
    return TimeNs{gap_us};
}

double TcpSwiftCC::get_cwnd() const { return m_cwnd; }

std::string TcpSwiftCC::to_string() const {
    return fmt::format(
        "[base_target: {} µs, target_now: {} µs, cwnd: {:0.3f}, "
        "ai: {:0.2f}, β: {:0.2f}]",
        m_base_target.value_nanoseconds(),
        compute_target_delay().value_nanoseconds(),
        m_cwnd, m_ai, m_beta_flow);
}

} // namespace sim
