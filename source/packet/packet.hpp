#pragma once

#include <string>

#include "flow/i_flow.hpp"
#include "routing_packet.hpp"
#include "utils/bitset.hpp"

namespace sim {

struct Packet : public RoutingPacket {
    Packet(Size a_size_byte = 0, IFlow* a_flow = nullptr, Id a_source_id = "",
           Id a_dest_id = "", Time a_sent_time = 0,
           Size a_sent_bytes_at_origin = 0, bool a_ecn_capable_transport = true,
           bool a_congestion_experienced = false);

    Packet(RoutingPacket a_routing_packet, IFlow* a_flow = nullptr,
           bool a_ecn_capable_transport = true,
           bool a_congestion_experienced = false);

    bool operator==(const Packet& packet) const;
    std::string to_string() const;

    BitSet<PacketFlagsBase> flags;
    IFlow* flow;
    bool ecn_capable_transport;
    bool congestion_experienced;
};

}  // namespace sim
