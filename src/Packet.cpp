#include "Packet.hpp"

Packet::Packet(std::uint64_t src_id, std::uint64_t dst_id, std::uint64_t sending_time_ns, bool is_ack):
    m_source_id(src_id),
    m_destination_id(dst_id),
    m_sending_time_ns(sending_time_ns),
    m_is_ack(is_ack) { }
