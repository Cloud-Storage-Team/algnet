#pragma once
#include <yaml-cpp/yaml.h>

#include "connection/flow/tcp/i_tcp_cc.hpp"

namespace sim {

class TcpCCParser {
public:
    static std::unique_ptr<ITcpCC> parse_i_tcp_cc(const YAML::Node& cc_node);
};

}  // namespace sim