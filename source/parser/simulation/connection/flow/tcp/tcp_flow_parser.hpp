#include <yaml-cpp/yaml.h>

#include "connection/flow/tcp/tcp_flow.hpp"
#pragma once

namespace sim {

class TcpFlowParser {
public:
    // ATTENTION: do not change std::shared_ptr with std::unique_ptr (because
    // TcpFlow is inherit from std::enable_shared_from_this)
    static std::shared_ptr<TcpFlow> parse_tcp_flow(const YAML::Node& key_node,
                                                   const YAML::Node& value_node,
                                                   Id conn_id);
};

}  // namespace sim