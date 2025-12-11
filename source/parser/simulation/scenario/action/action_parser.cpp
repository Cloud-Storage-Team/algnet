#include "action_parser.hpp"

#include <regex>
#include <sstream>

#include "parser/parse_utils.hpp"
#include "send_data_action_parser.hpp"
#include "stop_time_action_parser.hpp"
#include "utils/identifier_factory.hpp"

namespace sim {

ActionParser::ActionParser(std::shared_ptr<SendDataActionsSummary> a_summary)
    : m_send_data_parser(a_summary) {}

std::unique_ptr<IAction> ActionParser::parse(const ConfigNode& node) {
    const std::string action =
        node["action"].value_or_throw().as_or_throw<std::string>();
    if (action == "send_data") {
        return m_send_data_parser.parse(node);
    } else if (action == "stop_time") {
        return StopTimeActionParser::parse(node);
    }
    throw node.create_parsing_error("Unknown scenario action: " + action);
}

}  // namespace sim
