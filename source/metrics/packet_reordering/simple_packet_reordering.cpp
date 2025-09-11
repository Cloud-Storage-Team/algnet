#include "simple_packet_reordering.hpp"

namespace sim {

SimplePacketReordering::SimplePacketReordering(std::size_t a_max_packets)
    : m_max_packets(a_max_packets), m_inversions_count(0) {}

void SimplePacketReordering::add_record(PacketNum packet_num) {
    m_packet_num_set.insert(packet_num);
    std::size_t count_lower = m_packet_num_set.order_of_key(packet_num);
    std::size_t total_count = m_packet_num_set.size();
    std::size_t count_upper = total_count - count_lower - 1;

    m_inversions_count += count_upper;

    if (m_packet_num_set.size() > m_max_packets) {
        m_packet_num_set.erase(m_packet_num_set.begin());
    }
}

PacketReordering SimplePacketReordering::value() const {
    return m_inversions_count;
}

};  // namespace sim