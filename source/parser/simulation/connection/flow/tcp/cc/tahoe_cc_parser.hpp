#pragma once
#include <yaml-cpp/yaml.h>

#include "connection/flow/tcp/tahoe/tcp_tahoe_cc.hpp"

namespace sim {
class TahoeCCParser {
public:
    static std::unique_ptr<TcpTahoeCC> parse_tahoe_cc(const YAML::Node& node);
};
}  // namespace sim