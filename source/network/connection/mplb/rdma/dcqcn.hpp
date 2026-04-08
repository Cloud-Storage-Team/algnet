#pragma once
#include "types.hpp"

namespace sim {

// DQCCN congection control realization
// Based on NVIDIA documentation:
// https://enterprise-support.nvidia.com/s/article/DCQCN-CC-algorithm
// https://enterprise-support.nvidia.com/s/article/dcqcn-parameters

struct ParamsDQCCN {
    // -------------Rate increment-------------

    // The time period between rate increase events.
    TimeUs rpg_time_reset = TimeUs(300);

    // 	The sent bytes counter between rate increase events.
    SizeByte rpg_byte_reset = SizeByte(64 * 32767);

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
    SpeedMbps rpg_min_dec_fac = SpeedMbps(50);
};

class DCQCQN {
public:
    explicit DCQCQN(const ParamsDQCCN& a_params);
};

}  // namespace sim