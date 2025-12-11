#pragma once
#include "parser/config_reader/config_node.hpp"
#include "scenario/action/send_data_action.hpp"

namespace sim {

class SendDataActionParser {
public:
    SendDataActionParser(std::shared_ptr<SendDataActionsSummary> a_summary);

    std::unique_ptr<IAction> parse(const ConfigNode& node);

private:
    std::shared_ptr<SendDataActionsSummary> m_summary;
    std::set<RawDataId> m_data_ids;
};

}  // namespace sim
