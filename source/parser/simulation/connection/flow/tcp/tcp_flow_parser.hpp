#pragma once
#include "connection/flow/tcp/tcp_flow.hpp"
#include "parser/config_reader/config_node.hpp"

namespace sim {

class TcpFlowParser {
public:
    // ATTENTION: do not change std::shared_ptr with std::unique_ptr (because
    // TcpFlow is inherit from std::enable_shared_from_this)
    static std::shared_ptr<TcpFlow> parse_tcp_flow(const ConfigNode& node,
                                                   Id conn_id);
};

}  // namespace sim