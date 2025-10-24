#pragma once

#include <memory>
#include <vector>

#include "action/i_action.hpp"

namespace sim {

class Scenario {
public:
    Scenario() = default;
    ~Scenario() = default;

    Scenario(const Scenario&) = delete;
    Scenario& operator=(const Scenario&) = delete;

    Scenario(Scenario&&) noexcept = default;
    Scenario& operator=(Scenario&&) noexcept = default;

    // Add a new action to the scenario
    void add_action(std::unique_ptr<IAction> action);

    // Run all actions (schedule them in the simulator)
    void start();

private:
    std::vector<std::unique_ptr<IAction>> m_actions;
};

}  // namespace sim
