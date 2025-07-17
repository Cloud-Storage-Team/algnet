#include "flow/tcp/tahoe/tcp_tahoe_cc.hpp"
#include "parse_tcp_flow.hpp"

namespace sim {
template <>
TcpTahoeCC Parser<TcpFlow<TcpTahoeCC>>::parse_tcp_cc(
    [[maybe_unused]] const YAML::Node& key_node,
    [[maybe_unused]] const YAML::Node& value_node) {
    return TcpTahoeCC();
}

}  // namespace sim