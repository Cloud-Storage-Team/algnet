#include "simple_packet_reordering.hpp"

namespace sim {

SimplePacketReordering::SimplePacketReordering()
    : m_next_packet_num(0), m_reordering(0) {}

void SimplePacketReordering::add_record(PacketNum packet_num) {
    if (packet_num == m_next_packet_num) {
        m_next_packet_num++;
    } else {
        m_reordering++;
    }
}

PacketReordering SimplePacketReordering::value() const { return m_reordering; }

};  // namespace sim