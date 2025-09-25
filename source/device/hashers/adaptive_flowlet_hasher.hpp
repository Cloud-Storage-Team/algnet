#pragma once
#include "ecmp_hasher.hpp"

namespace sim {

class AdaptiveFlowletHasher : public IPacketHasher {
public:
    AdaptiveFlowletHasher() = default;
    ~AdaptiveFlowletHasher() = default;
    std::uint32_t get_hash(Packet packet) final;

private:
    // Maps flow ids to pair (last_time, shift)
    // where last_time is the last time packet from given flow was catched
    // and shift is a integer that should be addeded to ECMP hash for packets
    // from this flow
    std::map<Id, std::pair<TimeNs, std::uint32_t> > m_flow_table;

    ECMPHasher m_ecmp_hasher;
};

}  // namespace sim