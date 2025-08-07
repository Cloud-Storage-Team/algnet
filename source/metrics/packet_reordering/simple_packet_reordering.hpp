#pragma once
#include "i_packet_reordering.hpp"
#include "utils/ordered_set.hpp"
#include "utils/statistics.hpp"

namespace sim {
class SimplePacketReordering : public IPacketReordering {
public:
    SimplePacketReordering() = default;
    explicit SimplePacketReordering(long double a_smoothing_factor);
    SimplePacketReordering(long double a_smoothing_factor,
                           std::size_t a_max_packets);
    ~SimplePacketReordering() = default;

    void add_record(PacketNum packet_num) final;
    PacketReordering value() const final;

private:
    // maximal count of packets in m_packet_num_set
    // When m_packet_num_set.size() growth up it, the smallest element deletes
    std::size_t m_max_packets = 1'000'000;
    utils::ordered_set<PacketNum> m_packet_num_set;
    utils::Statistics<long double> m_metric_statistics;
};
}  // namespace sim