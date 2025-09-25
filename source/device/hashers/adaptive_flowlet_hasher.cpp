#include "adaptive_flowlet_hasher.hpp"
#include "scheduler.hpp"

namespace sim {
std::uint32_t AdaptiveFlowletHasher::get_hash(Packet packet) {
    std::uint32_t ecmp_hash = m_ecmp_hasher.get_hash(packet);

    IFlow* flow = packet.flow;

    if (!flow) {
        LOG_ERROR(
            "Try to use AdaptiveFlowletHasher on packet with not set flow");
        return ecmp_hash;
    }

    Id flow_id = flow->get_id();

    TimeNs curr_time = Scheduler::get_instance().get_current_time();
    auto it = m_flow_table.find(flow_id);
    if (it == m_flow_table.end()) {
        m_flow_table[flow_id] = {curr_time, 0};
        return ecmp_hash;
    }

    auto& [last_seen, shift] = it->second;
    TimeNs elapsed_from_last_seen = curr_time - last_seen;

    const FlowFlagsManager& flag_manager = flow->get_flag_mamager();
    auto flowlet_threshold = flag_manager.get_flag(packet.flags, "avg_rtt");
    if (flowlet_threshold == 0) {
        LOG_ERROR(
            "Adaptive flowlet hasher can not filnd avg rtt; returned previous "
            "hash");
        return ecmp_hash + shift;
    }

    if (elapsed_from_last_seen > TimeNs(flowlet_threshold)) {
        shift++;
    }
    last_seen = curr_time;

    return ecmp_hash + shift;
}
}  // namespace sim