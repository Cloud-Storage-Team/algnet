#include "round_robin_mplb.hpp"

namespace sim {

void RoundRobinMPLB::add_flow(std::shared_ptr<IFlow> flow) {
    m_flows.push_back(std::move(flow));
}

void RoundRobinMPLB::remove_flow(const std::shared_ptr<IFlow>& flow) {
    m_flows.erase(
        std::remove(m_flows.begin(), m_flows.end(), flow),
        m_flows.end()
    );

    if (m_next_index >= m_flows.size()) {
        m_next_index = 0;
    }
}

std::shared_ptr<IFlow> RoundRobinMPLB::select_flow() {
    if (m_flows.empty()) {
        return nullptr;
    }

    const std::size_t flows_count = m_flows.size();
    std::size_t checked = 0;

    while (checked < flows_count) {
        std::shared_ptr<IFlow> flow = m_flows[m_next_index];
        m_next_index = (m_next_index + 1) % flows_count;
        ++checked;

        if (flow->can_send()) {
            return flow;
        }
    }

    // No flow can send
    return nullptr;
}

void RoundRobinMPLB::notify_ack_received(const std::shared_ptr<IFlow>&, const FlowSample) {
    // Round-robin does not need an ACK update
}

void RoundRobinMPLB::notify_packet_sent(const std::shared_ptr<IFlow>&, const FlowSample) {
    // Round-robin doesn't need logic when sending
}

}  // namespace sim
