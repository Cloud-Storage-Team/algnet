#pragma once
#include "types.hpp"

namespace sim {

struct RTO {
    TimeNs current;
    TimeNs max;
    bool is_steady;

    constexpr RTO(TimeNs a_current, TimeNs a_max, bool a_is_steady = false)
        : current(a_current), max(a_max), is_steady(a_is_steady) {}
};

}  // namespace sim
