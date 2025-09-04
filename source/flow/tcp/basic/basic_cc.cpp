#include "basic_cc.hpp"

#include <limits>

namespace sim {

void BasicCC::on_ack([[maybe_unused]] TimeNs rtt,
                     [[maybe_unused]] TimeNs avg_rtt,
                     [[maybe_unused]] bool ecn_flag) {}

void BasicCC::on_timeout() {}

double BasicCC::get_cwnd() const { return 1e6; }

TimeNs BasicCC::get_pacing_delay() const { return TimeNs(0); }

SizeByte BasicCC::get_packet_size() const { return DEFAULT_PACKET_SIZE; };

std::string BasicCC::to_string() const { return ""; }

}  // namespace sim