#include "round_robin_path_chooser.hpp"

namespace sim {

RoundRobinPathChooser::RoundRobinPathChooser(FlowsSet a_flows)
    : m_flows(std::move(a_flows)),
      m_next_flow(m_flows.begin(), m_flows.end()) {}

std::shared_ptr<INewFlow> RoundRobinPathChooser::choose_flow() {
    if (m_flows.empty()) {
        throw std::runtime_error("Could not choose flow: flows set is empty");
    }
    return *m_next_flow++;
}

const IPathChooser::FlowsSet& RoundRobinPathChooser::get_flows() const {
    return m_flows;
}

}  // namespace sim
