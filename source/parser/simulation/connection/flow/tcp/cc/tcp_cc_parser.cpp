#include "tcp_cc_parser.hpp"

#include <spdlog/fmt/fmt.h>

#include "connection/flow/tcp/basic/basic_cc.hpp"
#include "connection/flow/tcp/tahoe/tcp_tahoe_cc.hpp"
#include "swift_cc_parser.hpp"

namespace sim {
std::unique_ptr<ITcpCC> TcpCCParser::parse_i_tcp_cc(const YAML::Node& node) {
    if (!node["type"]) {
        throw std::runtime_error("Missing 'cc.type' field");
    }

    std::string type = node["type"].as<std::string>();
    if (type == "basic") {
        return std::make_unique<BasicCC>();
    } else if (type == "tahoe") {
        return std::make_unique<TcpTahoeCC>();
    } else if (type == "swift") {
        return SwiftCCParser::parse_swift_cc(node);
    }
    throw std::runtime_error(
        fmt::format("Unexpected type of CC module: {}", type));
}
}  // namespace sim