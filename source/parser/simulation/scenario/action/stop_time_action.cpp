#include "action_parser.hpp"
#include "scenario/action/stop_time_action.hpp"

namespace sim{

    std::unique_ptr<IAction> ActionParser::parse_stop_time(const ConfigNode& node){
        const TimeNs time = parse_time(node["time"].value_or_throw());
        return std::make_unique<StopTimeAction>(time);
    }

} // namespace sim