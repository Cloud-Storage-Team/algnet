#include "scenario/action/stop_time_action.hpp"
#include "parser/config_reader/config_node.hpp"

namespace sim{

class StopTimeActionParser{

public:
    static std::unique_ptr<IAction> parse(const ConfigNode& node);
};

} // namespace sim