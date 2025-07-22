#include "basic_cc.hpp"

#include <limits>

namespace sim {

bool BasicCC::on_ack([[maybe_unused]] Time rtt, [[maybe_unused]] double avg_rtt,
                     [[maybe_unused]] bool ecn_flag) {
    return false;
}

double BasicCC::get_cwnd() const { return std::numeric_limits<double>::max(); }

Time BasicCC::get_pacing_delay() const { return 0; }

std::string BasicCC::to_string() const { return ""; }

}  // namespace sim