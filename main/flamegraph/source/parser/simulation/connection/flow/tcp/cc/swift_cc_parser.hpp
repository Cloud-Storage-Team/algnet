#pragma once
#include "connection/flow/tcp/swift/swift_cc.hpp"
#include "parser/config_reader/config_node.hpp"

namespace sim {

class SwiftCCParser {
public:
    static std::unique_ptr<TcpSwiftCC> parse_swift_cc(const ConfigNode& node);
};

}  // namespace sim