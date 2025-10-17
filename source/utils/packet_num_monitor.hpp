#pragma once
#include <optional>
#include <set>

#include "types.hpp"
namespace sim {
class PacketNumMonitor {
public:
    PacketNumMonitor();

    bool confirm_one(PacketNum packet_num);
    // Returns number of packets that packet_num confirms
    std::size_t confirm_to(PacketNum packet_num);
    bool is_confirmed(PacketNum packet_num) const;

    std::optional<PacketNum> get_last_confirmed() const;
    PacketNum get_first_unconfirmed() const;

private:
    // returns count of unconfirmed packets with numbers <= max_packet_num
    // carefully: work slowly!
    std::size_t get_unconfirmed_count(PacketNum max_packet_num) const;

    void correctify_state();

    // State: at any moment m_confirmed containts ONLY packet numbers >
    // m_first_unconfirmed
    std::set<PacketNum> m_confirmed;
    PacketNum m_first_unconfirmed;
};
}  // namespace sim