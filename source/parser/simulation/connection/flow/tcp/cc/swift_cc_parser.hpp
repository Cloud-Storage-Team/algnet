#pragma once
#include <yaml-cpp/yaml.h>

#include "connection/flow/tcp/swift/swift_cc.hpp"

namespace sim {

class SwiftCCParser {
public:
    static std::unique_ptr<TcpSwiftCC> parse_swift_cc(const YAML::Node& node);
};

}  // namespace sim