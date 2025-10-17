#include "packet_num_monitor.hpp"

namespace sim {
PacketNumMonitor::PacketNumMonitor() : m_confirmed(), m_first_unconfirmed(0) {}

bool PacketNumMonitor::confirm_one(PacketNum packet_num) {
    if (packet_num < m_first_unconfirmed) {
        // already confirmed
        return false;
    }
    bool confirmed = m_confirmed.insert(packet_num).second;
    if (confirmed && packet_num == m_first_unconfirmed) {
        correctify_state();
    }
    return confirmed;
}

std::size_t PacketNumMonitor::confirm_to(PacketNum packet_num) {
    size_t result = get_unconfirmed_count(packet_num);
    if (packet_num < m_first_unconfirmed) {
        // already confirmed
        return result;
    }

    m_first_unconfirmed = packet_num + 1;
    correctify_state();
    return result;
}

bool PacketNumMonitor::is_confirmed(PacketNum packet_num) const {
    if (m_first_unconfirmed > packet_num) {
        return true;
    }
    return m_confirmed.contains(packet_num);
}

std::size_t PacketNumMonitor::get_unconfirmed_count(
    PacketNum max_paket_num) const {
    if (max_paket_num < m_first_unconfirmed) {
        return 0;
    }
    std::size_t ans = max_paket_num - m_first_unconfirmed + 1;

    auto it = m_confirmed.begin();
    while (it != m_confirmed.end() && *it <= max_paket_num) {
        it++;
        ans--;
    }
    return ans;
}

std::optional<PacketNum> PacketNumMonitor::get_last_confirmed() const {
    return (m_first_unconfirmed > 0 ? std::optional(m_first_unconfirmed - 1)
                                    : std::nullopt);
}

PacketNum PacketNumMonitor::get_first_unconfirmed() const {
    return m_first_unconfirmed;
}

void PacketNumMonitor::correctify_state() {
    // correctify m_first_unconfirmed
    auto it = m_confirmed.lower_bound(m_first_unconfirmed);
    while (it != m_confirmed.end() && *it == m_first_unconfirmed) {
        m_first_unconfirmed++;
        it++;
    }
    // correctify m_confirmed
    m_confirmed.erase(m_confirmed.begin(), it);
}

}  // namespace sim