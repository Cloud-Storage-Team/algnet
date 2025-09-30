#include "action_parser.hpp"

#include <regex>
#include <sstream>

#include "parser/parse_utils.hpp"
#include "utils/identifier_factory.hpp"

namespace sim {

std::unique_ptr<IAction> ActionParser::parse(const YAML::Node& node) {
    auto action_node = node["action"];
    if (!action_node) {
        throw std::runtime_error("Scenario item must contain `action`");
    }
    const std::string action = action_node.as<std::string>();
    if (action == "send_data") {
        return parse_send_data(node);
    } else {
        throw std::runtime_error("Unknown scenario action: " + action);
    }
}

}  // namespace sim
