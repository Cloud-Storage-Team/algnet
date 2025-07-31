#include "swift_cc.hpp"

#include <spdlog/fmt/fmt.h>

#include <algorithm>
#include <cmath>
#include <string>

#include "scheduler.hpp"

namespace sim {

TcpSwiftCC::TcpSwiftCC(TimeNs a_base_target, long double a_additive_inc,
                       long double a_md_beta, long double a_max_mdf,
                       long double a_fs_range, long double a_fs_min_cwnd,
                       long double a_fs_max_cwnd)
    : m_base_target(a_base_target),
      m_ai(a_additive_inc),
      m_beta_md(a_md_beta),
      m_max_mdf(a_max_mdf),
      m_fs_range_ns(a_base_target * a_fs_range),
      m_fs_min_cwnd(a_fs_min_cwnd),
      m_fs_max_cwnd(a_fs_max_cwnd),
      m_cwnd(1.0L),
      m_last_decrease(Scheduler::get_instance().get_current_time()),
      m_last_rtt(a_base_target),
      m_retransmit_cnt(0) {
    // ---------- pre‑compute α and β for flow‑scaling ----------
    const long double inv_sqrt_min = 1.0 / std::sqrt(m_fs_min_cwnd);
    const long double inv_sqrt_max = 1.0 / std::sqrt(m_fs_max_cwnd);
    const long double denom = inv_sqrt_min - inv_sqrt_max;
    // α chosen so that flow_term == FS_RANGE at cwnd = FS_MIN_CWND
    m_alpha_flow = m_fs_range_ns / denom;
    m_beta_flow = m_alpha_flow / std::sqrt(m_fs_max_cwnd);
}

void TcpSwiftCC::on_ack(TimeNs rtt, [[maybe_unused]] TimeNs avg_rtt,
                        [[maybe_unused]] bool ecn_flag) {
    if (rtt < TimeNs(1.0L)) {
        throw std::runtime_error("Invalid rtt < 1");
    }
    m_last_rtt = rtt;

    // dynamic target_delay
    const TimeNs target_delay = compute_target_delay();

    const bool can_decrease = compute_can_decreace();
    long double new_cwnd = m_cwnd;
    // ---------- AIMD ----------
    if (rtt < target_delay) {
        // Additive‑Increase
        if (new_cwnd > 1.0L)
            new_cwnd += (m_ai / new_cwnd);
        else
            new_cwnd += m_ai;
    } else if (can_decrease) {
        // Multiplicative‑Decrease based on «overshoot»
        const long double overshoot = (rtt - target_delay) / rtt;
        const long double factor =
            std::max(1.0L - m_beta_md * overshoot, 1.0L - m_max_mdf);
        new_cwnd = std::max(M_MIN_CWND, new_cwnd * factor);
    }

    update_cwnd(new_cwnd);
}

void TcpSwiftCC::on_timeout() {
    const bool can_decrease = compute_can_decreace();
    m_retransmit_cnt++;
    long double new_cwnd = m_cwnd;
    if (m_retransmit_cnt >= M_RETX_RESET_THRESHOLD) {
        new_cwnd = M_MIN_CWND;
    } else if (can_decrease) {
        new_cwnd = (1 - m_max_mdf) * m_cwnd;
    }
    update_cwnd(new_cwnd);
}

TimeNs TcpSwiftCC::get_pacing_delay() const {
    if (m_cwnd > 1.0L) {
        return TimeNs(0);
    }
    long double cwnd_clamped = std::max(m_fs_min_cwnd, m_cwnd);
    return m_last_rtt / cwnd_clamped;
}

double TcpSwiftCC::get_cwnd() const { return (std::max(m_cwnd, 1.0L)); }

std::string TcpSwiftCC::to_string() const {
    return fmt::format("[target_now: {} ns, cwnd: {:0.3f}]",
                       compute_target_delay().value_nanoseconds(), m_cwnd);
}

TimeNs TcpSwiftCC::compute_target_delay() const {
    long double cwnd_clamped = std::max(m_fs_min_cwnd, m_cwnd);
    TimeNs flow_term = (m_alpha_flow / std::sqrt(cwnd_clamped)) - m_beta_flow;
    flow_term = std::clamp(flow_term, TimeNs(0.0L), m_fs_range_ns);
    return m_base_target + flow_term;  // TO-DO: use m_base_target +
                                       // TimeNs(flow_term) + h*hops_num
}

bool TcpSwiftCC::compute_can_decreace() const {
    TimeNs current_time = Scheduler::get_instance().get_current_time();
    return (current_time - m_last_decrease) > m_last_rtt;
}

void TcpSwiftCC::update_cwnd(long double new_cwnd) {
    new_cwnd = std::clamp(new_cwnd, M_MIN_CWND, M_MAX_CWND);
    if (new_cwnd < m_cwnd) {
        m_last_decrease = Scheduler::get_instance().get_current_time();
    }
    m_cwnd = new_cwnd;
}

}  // namespace sim
