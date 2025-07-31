#include "basic_cc.hpp"

#include <limits>

namespace sim {

bool BasicCC::on_ack([[maybe_unused]] TimeNs rtt,
                     [[maybe_unused]] TimeNs avg_rtt,
                     [[maybe_unused]] bool ecn_flag) {
    return false;
}

void BasicCC::on_timeout() {}

double BasicCC::get_cwnd() const { return std::numeric_limits<double>::max(); }

TimeNs BasicCC::get_pacing_delay() const { return TimeNs(0); }

std::string BasicCC::to_string() const { return ""; }

}  // namespace sim