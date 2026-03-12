#include "action_parser.hpp"

#include "send_data_action_parser.hpp"
#include "stop_action_parser.hpp"

namespace sim {
std::shared_ptr<IAction> parse_action(
    const ConfigNode& node,
    const utils::IdTable<IConnection> connections_table) {
    const std::string action_type =
        node["action"].value_or_throw().as_or_throw<std::string>();
    if (action_type == "send_data") {
        return parse_send_data_action(node, connections_table);
    } else if (action_type == "stop_time") {
        return parse_stop_time_action(node);
    }
    throw node.create_parsing_error("Unknown action type: " + action_type);
}
}  // namespace sim
