#include "switch_parser.hpp"

#include "packet_hasher_parser.hpp"
#include "parser/topology/ecn/ecn_parser.hpp"

namespace sim {

std::shared_ptr<ISwitch> parse_switch(
    const ConfigNodeWithPreset& preset,
    const ConfigNodeWithPreset& packet_spraying_node, const Id& id) {
    ConfigNodeWithPresetExpected ecn_node = preset["ecn"];
    ECN ecn(1.0, 1.0, 0.0);
    if (ecn_node.has_value()) {
        ecn = EcnParser::parse_ecn(ecn_node.value());
    }
    return std::make_shared<Switch>(
        id, std::move(ecn),
        parse_packet_hasher(packet_spraying_node.get_node(), id));
}

std::shared_ptr<ISwitch> SwitchParser::parse_i_switch(
    const ConfigNodeWithPreset& switch_node,
    const ConfigNodeWithPreset& packet_spraying_node) {
    Id id = switch_node.get_name_or_throw();
    return parse_switch(switch_node, packet_spraying_node, id);
}

}  // namespace sim
