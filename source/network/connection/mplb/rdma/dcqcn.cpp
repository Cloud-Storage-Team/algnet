#include "dcqcn.hpp"

#include "scheduler/scheduler.hpp"

namespace sim {
DCQCN::DCQCN(const ParamsDQCCN& a_params)
    : m_params(a_params),
      m_current_rate(a_params.rpg_min_rate),
      m_target_rate(m_current_rate),
      m_alpha(m_params.initial_alpha_value) {}

void DCQCN::start() {
    Scheduler& sched = Scheduler::get_instance();
    TimeNs now = sched.get_current_time();

    // enqueue first alpha update event
    sched.add(now + m_params.dce_tcp_rtt, [this]() { on_alpha_timer(); });

    // enqueue first rate increase timer event
    sched.add(now + m_params.rpg_time_reset, [this, last_cnp = m_last_cnp]() {
        on_rate_increase_timer(last_cnp);
    });

    sched.add(now + m_params.rate_reduce_monitor_period,
              [this]() { on_rate_reduce_monitor_period(); });
}

void DCQCN::stop() { m_stop_request = true; }

void DCQCN::on_cnp() {
    if (m_stop_request) {
        return;
    }

    TimeNs now = Scheduler::get_instance().get_current_time();
    m_last_cnp = now;
    m_bytes_from_last_byte_reset = SizeByte(0ul);
    m_time_counter = 0;
}

void DCQCN::on_data_delivery(SizeByte size) {
    if (m_stop_request) {
        return;
    }

    m_bytes_from_last_byte_reset += size;
    while (m_bytes_from_last_byte_reset >= m_params.rpg_byte_reset) {
        m_bytes_from_last_byte_reset -= m_params.rpg_byte_reset;
        m_byte_counter++;
        on_rate_increase_event();
    }
}

SpeedMbps DCQCN::get_rate() const { return m_current_rate; }

void DCQCN::on_rate_reduce_monitor_period() {
    if (m_stop_request) {
        return;
    }

    Scheduler& sched = Scheduler::get_instance();
    TimeNs now = sched.get_current_time();
    if (m_last_cnp &&
        now <= m_last_cnp.value() + m_params.rate_reduce_monitor_period) {
        // found CNP over last m_params.rate_reduce_monitor_period => reset
        // timers

        if (m_params.clamp_tgt_rate && !m_dec_target_rate) {
            m_target_rate = m_current_rate;
        }
        m_dec_target_rate = true;

        // decrement current rate
        m_current_rate =
            m_current_rate *
            std::max(m_params.rpg_min_dec_fac,
                     (1 - m_alpha / static_cast<double>(1 << m_params.rpg_gd)));
        m_current_rate = std::max(m_current_rate, m_params.rpg_min_rate);
        m_bytes_from_last_byte_reset = SizeByte(0ul);
        m_byte_counter = 0;
        m_time_counter = 0;
    }

    sched.add(now + m_params.rpg_time_reset, [this, last_cnp = m_last_cnp]() {
        on_rate_increase_timer(last_cnp);
    });
}

void DCQCN::on_alpha_timer() {
    if (m_stop_request) {
        return;
    }

    Scheduler& sched = Scheduler::get_instance();
    TimeNs now = sched.get_current_time();
    static constexpr int two_pow = (1 << 10);
    if (m_last_cnp && now <= m_last_cnp.value() + m_params.dce_tcp_rtt) {
        // cnp detected over last m_params.dce_tcp_rtt => increment alpha
        m_alpha =
            (m_params.dce_tcp_g / static_cast<double>(two_pow)) * m_alpha +
            two_pow - m_params.dce_tcp_g;
    } else {
        // no cnp over last last m_params.dce_tcp_rtt => decrement alpha
        m_alpha = (m_params.dce_tcp_g / static_cast<double>(two_pow)) * m_alpha;
    }
}

void DCQCN::on_rate_increase_timer(std::optional<TimeNs> last_elapced_cnp) {
    if (m_stop_request) {
        return;
    }

    if (m_last_cnp != last_elapced_cnp) {
        // there was cnp over m_params.rpg_time_reset => event should be
        // cancelled
        return;
    }
    // no cnp over last m_params.rpg_time_reset => update time counter &
    // reschedule event
    m_time_counter++;
    on_rate_increase_event();
    Scheduler& sched = Scheduler::get_instance();
    TimeNs now = sched.get_current_time();
    sched.add(now + m_params.rpg_time_reset,
              [this, new_last_elapced_cnp = m_last_cnp]() {
                  on_rate_increase_timer(new_last_elapced_cnp);
              });
}

void DCQCN::on_rate_increase_event() {
    if (m_stop_request) {
        return;
    }

    if (std::max(m_time_counter, m_byte_counter) < m_params.rpg_threshold) {
        // fast recovery; no target rate update
    } else if (std::min(m_time_counter, m_byte_counter) <=
               m_params.rpg_threshold) {
        // additive increase
        m_target_rate += m_params.rpg_ai_rate;
    } else {
        // hyper increase
        m_target_rate += m_params.rpg_hai_rate;
    }
    m_current_rate = (m_current_rate + m_target_rate) / 2.0;
}

}  // namespace sim