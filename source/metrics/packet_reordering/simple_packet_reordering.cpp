#include "simple_packet_reordering.hpp"

namespace sim {

SimplePacketReordering::SimplePacketReordering(long double a_smoothing_factor)
    : m_metric_statistics(a_smoothing_factor) {}

void SimplePacketReordering::add_record(PacketNum packet_num) {
    m_packet_num_set.insert(packet_num);

    std::size_t count_lower = m_packet_num_set.order_of_key(packet_num);
    std::size_t total_count = m_packet_num_set.size();
    std::size_t count_upper = total_count - count_lower - 1;

    long double curr_metric =
        count_upper / static_cast<long double>(total_count);
    m_metric_statistics.add_record(curr_metric);
}

PacketReordering SimplePacketReordering::value() const {
    return m_metric_statistics.get_mean();
}

};  // namespace sim