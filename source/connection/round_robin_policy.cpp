#include "round_robin_policy.hpp"

namespace sim {

RoundRobinPolicy::RoundRobinPolicy() = default;

std::shared_ptr<IFlow>
RoundRobinPolicy::select_flow(const std::vector<std::shared_ptr<IFlow>>& flows)
{
    if (flows.empty()) {
        return nullptr;
    }
    if (m_next >= flows.size()) {
        m_next = 0;
    }
    return flows[m_next++ % flows.size()];
}

} // namespace sim
