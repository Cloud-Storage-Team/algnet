#pragma once
#include "i_packet_reordering.hpp"
#include "utils/statistics.hpp"

namespace sim {

// Calculates count of packets that arrived not in right order
class SimplePacketReordering : public IPacketReordering {
public:
    SimplePacketReordering();

    void add_record(PacketNum packet_num) final;
    PacketReordering value() const final;

private:
    // maximal count of packets in m_packet_num_set
    // When m_packet_num_set.size() growth up it, the smallest element deletes
    std::size_t m_next_packet_num;
    PacketReordering m_reordering;
};
}  // namespace sim