#include "host_parser.hpp"

#include "parser/topology/ecn/ecn_parser.hpp"

namespace sim {

std::shared_ptr<IHost> HostParser::parse_i_host(const ConfigNodeWithPreset& host_node) {
    return parse_default_host(host_node);
}

std::shared_ptr<Host> HostParser::parse_default_host(
    const ConfigNodeWithPreset& host_node) {
    const Id id = host_node.get_node().get_name_or_throw();
    ConfigNodeWithPresetExpected exp_ecn_node = host_node["ecn"];
    ECN ecn(1.0, 1.0, 1.0);
    if (exp_ecn_node.has_value()) {
        ecn = EcnParser::parse_ecn(exp_ecn_node.value());
    }
    return std::make_shared<Host>(id, ecn);
}

}  // namespace sim
