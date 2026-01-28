#pragma once
#include "types.hpp"

namespace sim {

struct PacketAckInfo {
    TimeNs rtt;
    TimeNs avg_rtt;
    bool ecn_flag;
};

}  // namespace sim
