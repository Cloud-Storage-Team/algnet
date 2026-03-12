#include "stop_action_parser.hpp"

#include "parser/parse_utils.hpp"
#include "stop_action_parser.hpp"

namespace sim {

std::shared_ptr<NewStopTimeAction> parse_stop_time_action(
    const ConfigNode& node) {
    const TimeNs time = parse_time(node["time"].value_or_throw());
    return std::make_shared<NewStopTimeAction>(time);
}

}  // namespace sim