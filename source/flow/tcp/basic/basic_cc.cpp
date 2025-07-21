#include "basic_cc.hpp"

#include <limits>

namespace sim {

bool BasicCC::on_ack(TimeNs rtt, bool ecn_flag) {
    (void)rtt;
    (void)ecn_flag;
    return false;
}

double BasicCC::get_cwnd() const { return std::numeric_limits<double>::max(); }

TimeNs BasicCC::get_pacing_delay() const { return TimeNs(0); }

std::string BasicCC::to_string() const { return ""; }

}  // namespace sim