#include "adaptive_flowlet_hasher.hpp"

#include "scheduler.hpp"
#include "utils/avg_rtt_packet_flag.hpp"

namespace sim {

AdaptiveFlowletHasher::AdaptiveFlowletHasher(double a_factor)
    : m_factor(a_factor) {}

std::uint32_t AdaptiveFlowletHasher::get_hash(const Packet& packet) {
    std::uint32_t ecmp_hash = m_ecmp_hasher.get_hash(packet);

    IFlow* flow = packet.flow;

    if (!flow) {
        LOG_ERROR(
            fmt::format("Try to use AdaptiveFlowletHasher on packet {}; "
                        "flow does not set!",
                        packet.to_string()));
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

    auto exp_rtt = get_avg_rtt_label(packet.flags);
    if (!exp_rtt.has_value()) {
        LOG_ERROR(fmt::format("Adaptive flowlet hasher could not find avg rtt ({});"
                              "returned previous hash",
                              exp_rtt.error()));
        return ecmp_hash + shift;
    }
    TimeNs flowlet_threshold =
        exp_rtt.value() * m_factor;

    if (elapsed_from_last_seen > flowlet_threshold) {
        shift++;
    }
    last_seen = curr_time;

    return ecmp_hash + shift;
}

}  // namespace sim
