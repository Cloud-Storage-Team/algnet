#pragma once

#include <memory>
#include <vector>

#include "action/i_action.hpp"
#include "utils/actions_summary.hpp"

namespace sim {

class Scenario {
public:
    Scenario();

    Scenario(const Scenario&) = delete;
    Scenario& operator=(const Scenario&) = delete;

    Scenario(Scenario&&) noexcept = default;
    Scenario& operator=(Scenario&&) noexcept = default;

    // Add a new action to the scenario
    void add_action(std::shared_ptr<IAction> action);

    // Run all actions (schedule them in the simulator)
    void start();

    std::shared_ptr<SendDataActionsSummary> get_summary() const;

private:
    std::vector<std::shared_ptr<IAction>> m_actions;
    // TODO: move this to some other place (this class collect all type of
    // events, not only send_data, so it looks strange to store summary by onbly
    // one type of events here)
    std::shared_ptr<SendDataActionsSummary> m_send_data_actions_summary;
};

}  // namespace sim
