#include "tcp_cc_parser.hpp"

#include <spdlog/fmt/fmt.h>

#include "connection/flow/tcp/basic/basic_cc.hpp"
#include "swift_cc_parser.hpp"
#include "tahoe_cc_parser.hpp"

namespace sim {
std::unique_ptr<ITcpCC> TcpCCParser::parse_i_tcp_cc(const ConfigNode& node) {
    std::string type =
        node["type"].value_or_throw().as<std::string>().value_or_throw();
    if (type == "basic") {
        return std::make_unique<BasicCC>();
    } else if (type == "tahoe") {
        return TahoeCCParser::parse_tahoe_cc(node.get_node());
    } else if (type == "swift") {
        return SwiftCCParser::parse_swift_cc(node.get_node());
    }

    std::stringstream ss;
    ss << "Error while parsing node\n";
    ss << node.get_stacktrace() << '\n';
    ss << "Unexpected type of CC module: " << type;
    throw ConfigNodeError(ss.str());
}
}  // namespace sim