#include "round_robin_mplb.hpp"

namespace sim {

inline void RoundRobinMPLB::add_flow(const std::shared_ptr<IFlow>& flow) {
    if (!flow) return;
    if (m_flows.insert(flow).second && m_flows.size() == 1) {
        m_current_flow = {m_flows.begin(), m_flows.end()};
    }
}

inline void RoundRobinMPLB::remove_flow(const std::shared_ptr<IFlow>& flow) {
    if (!flow) return;

    if (auto it = m_flows.find(flow); it != m_flows.end()) {
        if (*m_current_flow == flow) {
            ++m_current_flow;
        }
        m_flows.erase(it);

        if (m_flows.empty()) {
            m_current_flow = {};
        } else {
            m_current_flow = {m_flows.begin(), m_flows.end()};
        }
    }
}

inline std::shared_ptr<IFlow> RoundRobinMPLB::select_flow() {
    if (m_flows.empty()) return nullptr;

    const auto n = m_flows.size();
    for (std::size_t i = 0; i < n; ++i) {
        auto flow = *m_current_flow;
        ++m_current_flow;
        if (flow && flow->can_send()) return flow;
    }
    return nullptr;
}

void RoundRobinMPLB::notify_packet_confirmed(const std::shared_ptr<IFlow>&,
                                             FlowSample) {
    // no-op for RR
}

} // namespace sim
