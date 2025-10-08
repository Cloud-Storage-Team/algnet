#pragma once
#include <spdlog/fmt/fmt.h>

#include <memory>

#include "connection/i_connection.hpp"
#include "parser/config_reader/config_node.hpp"

namespace sim {

class ConnectionParser {
public:
    static std::shared_ptr<IConnection> parse_i_connection(
        const ConfigNode& node);

private:
    static std::shared_ptr<IConnection> parse_connection(
        const ConfigNode& node);
};

}  // namespace sim
