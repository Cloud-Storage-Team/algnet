#include "basic_cc.hpp"

#include <limits>

namespace sim {

void BasicCC::on_ack([[maybe_unused]] const PacketAckInfo& info) {}

void BasicCC::on_timeout() {}

double BasicCC::get_cwnd() const { return 1e6; }

TimeNs BasicCC::get_pacing_delay() const { return TimeNs(0); }

std::string BasicCC::to_string() const { return ""; }

}  // namespace sim