#pragma once

#include <string>

#include "utils/bitset.hpp"
#include "flow/i_flow.hpp"

namespace sim {

struct Packet {
    Packet(Size a_size_byte = 0,
           IFlow* a_flow = nullptr, Id a_source_id = "", Id a_dest_id = "",
           Time a_sent_time = 0, Size a_sent_bytes_at_origin = 0,
           bool a_ecn_capable_transport = true,
           bool a_congestion_experienced = false);

    bool operator==(const Packet& packet) const;
    std::string to_string() const;

    std::uint32_t packet_num;
    BitSet<PacketFlagsBase> flags;
    Id source_id;
    Id dest_id;
    Size size_byte;
    IFlow* flow;
    Time sent_time;  // Note: ACK's sent time is the data packet sent time
    Size sent_bytes_at_origin;  // For ACK this is inherited from data packet
    bool ecn_capable_transport;
    bool congestion_experienced;
};

}  // namespace sim
