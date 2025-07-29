#include "swift_cc.hpp"
#include "scheduler.hpp"

#include <spdlog/fmt/fmt.h>
#include <algorithm>
#include <cmath>
#include <string>

namespace sim {

TcpSwiftCC::TcpSwiftCC(TimeNs a_base_target,
                       double  a_additive_inc,
                       double  a_md_beta,
                       double  a_max_mdf,
                       double  a_fs_range,
                       double  a_fs_min_cwnd,
                       double  a_fs_max_cwnd)
    : m_base_target(a_base_target),
      m_ai(a_additive_inc),
      m_beta_md(a_md_beta),
      m_max_mdf(a_max_mdf),
      m_fs_range_ns(a_fs_range * a_base_target.value_nanoseconds()),
      m_fs_min_cwnd(a_fs_min_cwnd),
      m_fs_max_cwnd(a_fs_max_cwnd),
      m_cwnd(1.0L),
      m_last_decrease(Scheduler::get_instance().get_current_time()),
      m_last_rtt(a_base_target) {

    // ---------- pre‑compute α and β for flow‑scaling ----------
    const long double inv_sqrt_min = 1.0 / std::sqrt(m_fs_min_cwnd);
    const long double inv_sqrt_max = 1.0 / std::sqrt(m_fs_max_cwnd);
    const long double denom        = inv_sqrt_min - inv_sqrt_max;
    // α chosen so that flow_term == FS_RANGE at cwnd = FS_MIN_CWND
    m_alpha = m_fs_range_ns.value_nanoseconds() / denom;
    m_beta_flow = -(m_alpha / std::sqrt(m_fs_max_cwnd));   // ≤ 0
}

bool TcpSwiftCC::on_ack(TimeNs rtt,
                        [[maybe_unused]] TimeNs avg_rtt,
                        [[maybe_unused]] bool ecn_flag) {
    if (rtt < TimeNs(1.0L))
        throw std::runtime_error("Invalid rtt < 1");
    m_last_rtt = rtt;

    // dynamic target_delay
    const TimeNs target_delay = compute_target_delay();

    TimeNs current_time = Scheduler::get_instance().get_current_time();
    const bool can_decrease  = (current_time - m_last_decrease) > rtt;

    // ---------- AIMD ----------
    if (rtt < target_delay) {
        // Additive‑Increase
        if (m_cwnd > 1.0L)
            m_cwnd += (m_ai / m_cwnd);
        else
            m_cwnd += m_ai;
    } else if (can_decrease) {
        // Multiplicative‑Decrease based on «overshoot»
        const long double overshoot = (rtt - target_delay) / rtt;
        const long double factor = std::max(1.0L - m_beta_md * overshoot, 1.0L - m_max_mdf);
        m_cwnd = std::max(MIN_CWND, m_cwnd * factor);
        m_last_decrease = current_time;
    }

    m_cwnd = std::clamp(m_cwnd, MIN_CWND, MAX_CWND);
    return true;
}

TimeNs TcpSwiftCC::get_pacing_delay() const {
    if (m_cwnd > 1.0L) {
        return TimeNs(0);
    }     
    long double cwnd_clamped = std::max(m_fs_min_cwnd, m_cwnd);
    long double gap_ns = m_last_rtt.value_nanoseconds() / cwnd_clamped;
    return TimeNs(gap_ns);
}

double TcpSwiftCC::get_cwnd() const {
    return (std::max(m_cwnd, 1.0L));
}

std::string TcpSwiftCC::to_string() const {
    return fmt::format(
        "[target_now: {} ns, cwnd: {:0.3f}]",
        compute_target_delay().value_nanoseconds(),
        m_cwnd);
}

TimeNs TcpSwiftCC::compute_target_delay() const {
    long double cwnd_clamped = std::max(m_fs_min_cwnd, m_cwnd);
    long double flow_term = (m_alpha / std::sqrt(cwnd_clamped)) + m_beta_flow;
    flow_term = std::clamp(flow_term, 0.0L, m_fs_range_ns.value_nanoseconds());
    return m_base_target + TimeNs(flow_term); // TO-DO: use m_base_target + TimeNs(flow_term) + h*hops_num
}

} // namespace sim
