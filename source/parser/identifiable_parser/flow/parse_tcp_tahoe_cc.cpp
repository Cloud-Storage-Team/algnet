#include "flow/tcp/aimd/tcp_aimd_cc.hpp"
#include "parse_tcp_flow.hpp"

namespace sim {
template <>
TcpAIMD_CC Parser<TcpFlow<TcpAIMD_CC>>::parse_tcp_cc(
    [[maybe_unused]] const YAML::Node& key_node,
    [[maybe_unused]] const YAML::Node& value_node) {
    return TcpAIMD_CC();
}

}  // namespace sim