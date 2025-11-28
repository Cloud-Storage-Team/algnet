#include "scenario/action/send_data_action.hpp"
#include "parser/config_reader/config_node.hpp"

namespace sim {

class SendDataActionParser {
public:
    static std::unique_ptr<IAction> parse(const ConfigNode& node);
};

} // namespace sim
