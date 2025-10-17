#pragma once
#include <set>

#include "types.hpp"
namespace sim {
class AckMonitor {
public:
    AckMonitor();

    void confirm_one(PacketNum packet_num);
    void confirm_to(PacketNum packet_num);
    bool is_confirmed(PacketNum packet_num) const;

private:
    void correctify_state();

    // State: at any moment m_confirmed_mex contaits mex of all confirmed (got
    // by confirm_one and confirm_to) packets, m_confirmed containts ONLY packet
    // numbers > m_confirmed_mex
    std::set<PacketNum> m_confirmed;
    PacketNum m_confirmed_mex;
};
}  // namespace sim