#include "scenario.hpp"

#include <memory>

#include "action/send_data_action.hpp"
#include "scheduler/scheduler.hpp"

namespace sim {

Scenario::Scenario(Actions a_actions, Network a_network)
    : m_actions(std::move(a_actions)), m_network(std::move(a_network)) {}

Summary Scenario::simulate() {
    m_network.recalculate_pathes();

    for (auto action : m_actions) {
        action->schedule();
    }

    while (Scheduler::get_instance().tick()) {
    }

    Summary summary;

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

const Network& Scenario::get_network() const { return m_network; }

}  // namespace sim
