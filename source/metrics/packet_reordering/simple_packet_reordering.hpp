#pragma once
#include "i_packet_reordering.hpp"
#include "utils/ordered_set.hpp"
#include "utils/statistics.hpp"

namespace sim {
class SimplePacketReordering : public IPacketReordering {
public:
    SimplePacketReordering(long double a_smoothing_factor = 0.8);
    ~SimplePacketReordering() = default;

    void add_record(PacketNum packet_num) final;
    PacketReordering value() const final;

private:
    utils::ordered_set<PacketNum> m_packet_num_set;
    utils::Statistics<long double> m_metric_statistics;
};
}  // namespace sim