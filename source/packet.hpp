#pragma once

#include <string>

#include "flow/i_flow.hpp"
#include "utils/bitset.hpp"

namespace sim {

struct Packet {
    Packet(SizeByte a_size_byte = SizeByte(0), IFlow* a_flow = nullptr,
           Id a_source_id = "", Id a_dest_id = "", TimeNs a_sent_time = 0,
           SizeByte a_sent_bytes_at_origin = SizeByte(0),
           bool a_ecn_capable_transport = true,
           bool a_congestion_experienced = false);

    bool operator==(const Packet& packet) const;
    std::string to_string() const;

    std::uint32_t packet_num;
    BitSet<PacketFlagsBase> flags;
    Id source_id;
    Id dest_id;
    SizeByte size_byte;
    IFlow* flow;
    TimeNs sent_time;  // Note: ACK's sent time is the data packet sent time
    SizeByte
        sent_bytes_at_origin;  // For ACK this is inherited from data packet
    bool ecn_capable_transport;
    bool congestion_experienced;
};

}  // namespace sim
