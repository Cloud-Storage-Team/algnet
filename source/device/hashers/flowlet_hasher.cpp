#include "flowlet_hasher.hpp"

#include "scheduler.hpp"

namespace sim {

FLowletHasher::FLowletHasher(TimeNs a_flowlet_threshold)
    : m_flowlet_threshold(a_flowlet_threshold) {}

std::uint32_t FLowletHasher::get_hash(const Packet& packet) {
    std::uint32_t ecmp_hash = m_ecmp_hasher.get_hash(packet);

    Id flow_id = packet.flow_id;
    TimeNs curr_time = Scheduler::get_instance().get_current_time();
    auto it = m_flow_table.find(flow_id);

    if (it == m_flow_table.end()) {
        m_flow_table[flow_id] = {curr_time, 0};
        return ecmp_hash;
    }

    auto& [last_seen, shift] = it->second;
    TimeNs elapsed_from_last_seen = curr_time - last_seen;

    if (elapsed_from_last_seen > m_flowlet_threshold) {
        shift++;
    }
    last_seen = curr_time;

    return ecmp_hash + shift;
}

}  // namespace sim