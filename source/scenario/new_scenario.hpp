#pragma once

#include <memory>
#include <vector>

#include "action/i_new_action.hpp"
#include "network/network.hpp"
#include "utils/actions_summary.hpp"

namespace sim {

using Actions = std::vector<std::shared_ptr<INewAction>>;

struct NewSummary {
    SendDataActionsSummary send_data;
};

// Launch scenaro: network & actions over it
class NewScenario {
public:
    NewScenario(Actions a_actions, Network a_network);

    NewSummary simulate();

    const Network& get_network() const;

private:
    Actions m_actions;
    Network m_network;
};

}  // namespace sim
