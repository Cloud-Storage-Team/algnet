#pragma once

#include <memory>
#include <vector>

#include "action/i_action.hpp"
#include "network/network.hpp"
#include "utils/actions_summary.hpp"

namespace sim {

using Actions = std::vector<std::shared_ptr<IAction>>;

struct Summary {
    SendDataActionsSummary send_data;
};

// Launch scenaro: network & actions over it
class Scenario {
public:
    Scenario(Actions a_actions, Network a_network);

    Summary simulate();

    const Network& get_network() const;

private:
    Actions m_actions;
    Network m_network;
};

}  // namespace sim
