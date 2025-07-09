#pragma once
#include "types.hpp"

namespace sim {

struct PacketHeader {
    PacketHeader(Size a_size = 0, Id a_source_id = "", Id a_dest_id = "",
                 Size a_sent_bytes_at_origin = 0, Time a_send_time = 0);

    Size size;
    Id source_id;
    Id dest_id;
    Size sent_bytes_at_origin;  // For ACK this is inherited from data packet
    Time sent_time;  // Note: ACK's sent time is the data packet sent time
};
}  // namespace sim
