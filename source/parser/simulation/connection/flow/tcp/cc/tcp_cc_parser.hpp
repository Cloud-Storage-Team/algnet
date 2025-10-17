#pragma once
#include "connection/flow/tcp/cc/i_tcp_cc.hpp"
#include "parser/config_reader/config_node.hpp"

namespace sim {

class TcpCCParser {
public:
    static std::unique_ptr<ITcpCC> parse_i_tcp_cc(const ConfigNode& node);
};

}  // namespace sim