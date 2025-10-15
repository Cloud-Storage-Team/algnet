#pragma once
#include "connection/flow/tcp/tahoe/tcp_tahoe_cc.hpp"
#include "parser/config_reader/config_node.hpp"

namespace sim {
class TahoeCCParser {
public:
    static std::unique_ptr<TcpTahoeCC> parse_tahoe_cc(const ConfigNode& node);
};
}  // namespace sim