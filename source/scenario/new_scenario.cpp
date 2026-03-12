#include "new_scenario.hpp"

#include <memory>

#include "action/send_data_action.hpp"
#include "scheduler.hpp"

namespace sim {

NewScenario::NewScenario(Actions a_actions, Network a_network)
    : m_actions(std::move(a_actions)), m_network(std::move(a_network)) {}

NewSummary NewScenario::simulate() {
    m_network.recalculate_pathes();

    for (auto action : m_actions) {
        action->schedule();
    }

    while (Scheduler::get_instance().tick()) {
    }

    NewSummary summary;

    for (auto action : m_actions) {
        if (auto send_data_action =
                std::dynamic_pointer_cast<NewSendDataAction>(action);
            send_data_action) {
            for (const auto& row : send_data_action->get_summary()) {
                summary.send_data.emplace_back(row);
            }
        }
    }

    return summary;
}

const Network& NewScenario::get_network() const { return m_network; }

}  // namespace sim
