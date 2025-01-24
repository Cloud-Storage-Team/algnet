#include "Packet.hpp"

Packet::Packet(std::uint64_t src_id, std::uint64_t dest_id, std::uint64_t sending_time_ns, bool is_ack):
    source_id(src_id),
    destination_id(dest_id),
    sending_time_ns(sending_time_ns),
    is_ack(is_ack) { }
