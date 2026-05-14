#pragma once
#include <optional>

#include "types.hpp"

namespace sim {

// DQCCN congestion control realization
// Based on NVIDIA documentation:
// https://enterprise-support.nvidia.com/s/article/DCQCN-CC-algorithm
// https://enterprise-support.nvidia.com/s/article/dcqcn-parameters

struct ParamsDQCCN {
    SpeedGbps start_target_rate;
    // -------------Rate increment-------------

    // The time period between rate increase events.
    TimeUs rpg_time_reset = TimeUs(300);

    // 	The sent bytes counter between rate increase events.
    SizeByte rpg_byte_reset = SizeByte(64 * 32767ul);

    // The threshold of rate increase events for moving to next rate increase
    // phase.
    std::size_t rpg_threshold = 1;

    // The rate increase value in the Additive Increase phase.
    SpeedMbps rpg_ai_rate = SpeedMbps(5);

    // The rate increase value in the Hyper Increase phase.
    SpeedMbps rpg_hai_rate = SpeedMbps(50);

    // -------------Alpha update-------------

    // This parameter sets the initial value of alpha that should be used when
    // receiving the first CNP for a flow.
    int initial_alpha_value = 1023;

    // Controls aggressiveness of alpha's updates
    // The lower G is, the more aggressive are the changes.
    int dce_tcp_g = 1019;

    // The Time period between alpha updates.
    TimeUs dce_tcp_rtt = TimeUs(1);

    // -------------Rate decrement-------------

    // The time period between rate reductions.
    TimeUs rate_reduce_monitor_period = TimeUs(4);

    // Rates (current, target) on first CNP (0 – 85% of line rate).
    SpeedMbps rate_to_set_on_first_cnp = SpeedMbps(0);

    // If true, every rate decreases. The target rate is updated to the current
    // rate.
    // Otherwise, the target rate is updated to the current rate only on the
    // first decrement after the increment event.
    bool clamp_tgt_rate = false;

    // The coefficient between alpha and the rate reduction factor.
    // Log2 of value in fixed point with 10 in the fraction part
    int rpg_gd = 11;

    // Minimal rate limit of the QP.
    SpeedMbps rpg_min_rate = SpeedMbps(1);
    // Maximal rate limit of the QP.
    double rpg_min_dec_fac = 0.5;
};

class DCQCN {
public:
    explicit DCQCN(const ParamsDQCCN& a_params);

    // enqueue initial events
    void start();

    // stop creating new events
    void stop();

    // Calls when sender got congestion notification
    void on_cnp();

    // Calls when sender got asknowledge of receiving data_size data
    void on_data_delivery(SizeByte data_size);

    SpeedMbps get_rate() const;

private:
    void on_rate_reduce_monitor_period();
    void on_alpha_timer();

    void on_rate_increase_timer(std::optional<TimeNs> last_elapced_cnp);
    void on_rate_increase_event();

    ParamsDQCCN m_params;
    SpeedMbps m_current_rate;
    SpeedMbps m_target_rate;
    bool m_dec_target_rate = false;

    // Size & time counters (T & BC on Increment scheme part of NVIDIA docs)
    SizeByte m_bytes_from_last_byte_reset = SizeByte(0ul);
    std::uint32_t m_time_counter = 0;
    std::uint32_t m_byte_counter = 0;

    // Last time CNP was got
    std::optional<TimeNs> m_last_cnp = std::nullopt;

    int m_alpha;
    bool m_stop_request = false;
};

}  // namespace sim
