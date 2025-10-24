#include "host_parser.hpp"

namespace sim {

std::shared_ptr<IHost> HostParser::parse_i_host(const ConfigNode& host_node) {
    return parse_default_host(host_node);
}

std::shared_ptr<Host> HostParser::parse_default_host(
    const ConfigNode& host_node) {
    return std::make_shared<Host>(host_node.get_name_or_throw());
}

}  // namespace sim
