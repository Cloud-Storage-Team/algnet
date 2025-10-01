#include "scenario.hpp"

namespace sim {

void Scenario::add_action(std::unique_ptr<IAction> action) {
    m_actions.emplace_back(std::move(action));
}

void Scenario::start() {
    for (auto& action : m_actions) {
        action->schedule();
    }
}

}  // namespace sim
