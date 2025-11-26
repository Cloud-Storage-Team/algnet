#include "scenario.hpp"

namespace sim {

Scenario::Scenario()
    : m_send_data_actions_summary(std::make_shared<SendDataActionsSummary>()) {}

void Scenario::add_action(std::shared_ptr<IAction> action) {
    m_actions.emplace_back(std::move(action));
}

std::shared_ptr<SendDataActionsSummary> Scenario::get_summary() const {
    return m_send_data_actions_summary;
}

void Scenario::start() {
    for (auto& action : m_actions) {
        action->schedule();
    }
}

}  // namespace sim
