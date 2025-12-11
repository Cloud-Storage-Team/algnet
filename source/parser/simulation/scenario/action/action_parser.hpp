#pragma once

#include <memory>

#include "parser/config_reader/config_node.hpp"
#include "send_data_action_parser.hpp"
#include "utils/actions_summary.hpp"

namespace sim {

class ActionParser {
public:
    ActionParser(std::shared_ptr<SendDataActionsSummary> a_summary);
    // Parse one YAML node into an IAction
    std::unique_ptr<IAction> parse(const ConfigNode& node);

private:
    SendDataActionParser m_send_data_parser;
};

}  // namespace sim
