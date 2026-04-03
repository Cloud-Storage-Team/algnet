#include "host_parser.hpp"

#include "parser/topology/ecn/ecn_parser.hpp"
namespace sim {

std::shared_ptr<IHost> parse_host(const ConfigNodeWithPreset& preset,
                                  const Id& id) {
    ConfigNodeWithPresetExpected exp_ecn_node = preset["ecn"];
    ECN ecn(1.0, 1.0, 1.0);
    if (exp_ecn_node.has_value()) {
        ecn = EcnParser::parse_ecn(exp_ecn_node.value());
    }
    return std::make_shared<Host>(id, ecn);
}

std::shared_ptr<IHost> HostParser::parse_i_host(
    const ConfigNodeWithPreset& host_node) {
    return parse_host(host_node, host_node.get_name_or_throw());
}

}  // namespace sim
