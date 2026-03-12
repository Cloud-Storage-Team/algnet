#include "round_robin_path_chooser.hpp"

namespace sim {

RoundRobinPathChooser::RoundRobinPathChooser(FlowsTable a_flows)
    : m_flows(std::move(a_flows)),
      m_next_flow(m_flows.begin(), m_flows.end()) {}

std::shared_ptr<IFlow> RoundRobinPathChooser::choose_flow() {
    if (m_flows.empty()) {
        throw std::runtime_error("Could not choose flow: flows set is empty");
    }
    std::shared_ptr<IFlow> result = (*m_next_flow).second;
    m_next_flow++;
    return result;
}

const IPathChooser::FlowsTable& RoundRobinPathChooser::get_flows_table() const {
    return m_flows;
}

}  // namespace sim
