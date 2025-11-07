#include "tcp_cc_parser.hpp"

#include <spdlog/fmt/fmt.h>

#include "connection/flow/tcp/basic/basic_cc.hpp"
#include "swift_cc_parser.hpp"
#include "tahoe_cc_parser.hpp"

namespace sim {
std::unique_ptr<ITcpCC> TcpCCParser::parse_i_tcp_cc(const ConfigNode& node) {
    std::string type = node["type"].value_or_throw().as_or_throw<std::string>();
    if (type == "basic") {
        return std::make_unique<BasicCC>();
    } else if (type == "tahoe") {
        return TahoeCCParser::parse_tahoe_cc(node);
    } else if (type == "swift") {
        return SwiftCCParser::parse_swift_cc(node);
    }

    throw node.create_parsing_error("Unexpected type of CC module: " + type);
}
}  // namespace sim