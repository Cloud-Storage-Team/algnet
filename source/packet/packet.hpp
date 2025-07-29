#pragma once

#include <string>

#include "flow/i_flow.hpp"
#include "route.hpp"
#include "utils/bitset.hpp"

namespace sim {

struct Packet {
    Packet(SizeByte a_size = SizeByte(0), IFlow* a_flow = nullptr,
           Route a_route = Route(), TimeNs a_sent_time = TimeNs(0),
           SizeByte a_delivered_at_origin = SizeByte(0),
           bool a_ecn_capable_transport = true,
           bool a_congestion_experienced = false);

    bool operator==(const Packet& packet) const;
    std::string to_string() const;

    std::uint32_t packet_num;
    BitSet<PacketFlagsBase> flags;
    Route route;
    SizeByte size;
    IFlow* flow;
    TimeNs sent_time;  // Note: ACK's sent time is the data packet sent time
    SizeByte delivered_data_size_at_origin;  // For ACK this is inherited from
                                             // data packet
    bool ecn_capable_transport;
    bool congestion_experienced;
};

}  // namespace sim
