#include "action_parser.hpp"

#include <regex>
#include <sstream>

#include "parser/parse_utils.hpp"
#include "utils/identifier_factory.hpp"

namespace sim {

std::unique_ptr<IAction> ActionParser::parse(const ConfigNode& node) {
    const std::string action =
        node["action"].value_or_throw().as_or_throw<std::string>();
    if (action == "send_data") {
        return parse_send_data(node);
    }
    else if (action == "stop_time"){
        return parse_stop_time(node);
    }
    throw node.create_parsing_error("Unknown scenario action: " + action);
}

}  // namespace sim
