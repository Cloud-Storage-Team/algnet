#pragma once

#include <memory>

#include "parser/config_reader/config_node.hpp"
#include "parser/parse_utils.hpp"
#include "scenario/action/i_action.hpp"
#include "utils/actions_summary.hpp"

namespace sim {

class ActionParser {
public:
    ActionParser(std::shared_ptr<SendDataActionsSummary> a_summary);
    // Parse one YAML node into an IAction
    std::unique_ptr<IAction> parse(const ConfigNode& node);

private:
    std::unique_ptr<IAction> parse_send_data(const ConfigNode& node);

    std::shared_ptr<SendDataActionsSummary> m_summary;
    std::set<DataId> m_data_ids;
};

}  // namespace sim
