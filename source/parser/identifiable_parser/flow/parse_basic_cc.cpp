#include "flow/tcp/basic/basic_cc.hpp"
#include "parse_tcp_flow.hpp"

namespace sim {
template <>
BasicCC IdentifieableParser<TcpFlow<BasicCC>>::parse_tcp_cc(
    const YAML::Node& key_node, const YAML::Node& value_node) {
    (void)key_node;
    (void)value_node;
    return BasicCC();
}

}  // namespace sim