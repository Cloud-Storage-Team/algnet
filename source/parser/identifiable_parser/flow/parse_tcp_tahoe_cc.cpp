#include "flow/tcp/tcp_tahoe_cc.hpp"
#include "parse_tcp_flow.hpp"

namespace sim {
template <>
TcpTahoeCC IdentifieableParser<TcpFlow<TcpTahoeCC>>::parse_tcp_cc(
    const YAML::Node& key_node, const YAML::Node& value_node) {
    (void)key_node;
    (void)value_node;
    return TcpTahoeCC();
}

}  // namespace sim