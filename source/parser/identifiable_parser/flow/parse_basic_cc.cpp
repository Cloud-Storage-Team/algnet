#include "flow/tcp/basic/basic_cc.hpp"
#include "parse_tcp_flow.hpp"

namespace sim {
template <>
BasicCC IdentifieableParser<TcpFlow<BasicCC>>::parse_tcp_cc(
    [[maybe_unused]] const YAML::Node& key_node,
    [[maybe_unused]] const YAML::Node& value_node) {
    return BasicCC();
}

}  // namespace sim