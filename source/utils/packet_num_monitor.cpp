#include "packet_num_monitor.hpp"

namespace sim {
PacketNumMonitor::PacketNumMonitor() : m_confirmed(), m_confirmed_mex(0) {}

void PacketNumMonitor::confirm_one(PacketNum packet_num) {
    if (packet_num < m_confirmed_mex) {
        // already confirmed
        return;
    }
    if (m_confirmed.insert(packet_num).second &&
        packet_num == m_confirmed_mex) {
        correctify_state();
    }
}

void PacketNumMonitor::confirm_to(PacketNum packet_num) {
    if (packet_num < m_confirmed_mex) {
        // already confirmed
        return;
    }

    m_confirmed_mex = packet_num + 1;
    correctify_state();
}

bool PacketNumMonitor::is_confirmed(PacketNum packet_num) const {
    if (m_confirmed_mex > packet_num) {
        return true;
    }
    return m_confirmed.contains(packet_num);
}

void PacketNumMonitor::correctify_state() {
    // correctify m_confirmed_mex
    auto it = m_confirmed.lower_bound(m_confirmed_mex);
    while (it != m_confirmed.end() && *it == m_confirmed_mex) {
        m_confirmed_mex++;
        it++;
    }
    // correctify m_confirmed
    m_confirmed.erase(m_confirmed.begin(), it);
}

}  // namespace sim