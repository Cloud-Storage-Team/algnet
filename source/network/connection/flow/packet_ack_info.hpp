#pragma once
#include "packet.hpp"
#include "types.hpp"
#include "utils/statistics.hpp"

namespace sim {

struct PacketAckInfo {
    TimeNs rtt;
    const utils::Statistics<TimeNs>& rtt_stat;
    const Packet& ack;
};

}  // namespace sim
