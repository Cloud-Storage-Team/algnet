#include "flow/basic_flow.hpp"
#include "identifiable_parser.hpp"
#include "parse_flow_common.hpp"

namespace sim {

template <>
std::shared_ptr<BasicFlow> IdentifieableParser::parse_object(
    const YAML::Node& key_node, const YAML::Node& value_node) {
    FlowCommon flow_common = parse_flow_common(key_node, value_node);

    return std::make_shared<BasicFlow>(
        flow_common.id, dynamic_pointer_cast<IHost>(flow_common.sender_ptr),
        dynamic_pointer_cast<IHost>(flow_common.receiver_ptr),
        flow_common.packet_size, flow_common.packet_interval,
        flow_common.number_of_packets);
}

}  // namespace sim