#pragma once
#include <spdlog/fmt/fmt.h>

#include <memory>

#include "connection/i_connection.hpp"
#include "parser/config_reader/config_node.hpp"

namespace sim {

class ConnectionParser {
public:
    static std::shared_ptr<IConnection> parse_i_connection(
        const YAML::Node& key_node, const YAML::Node& value_node);

private:
    static std::shared_ptr<IConnection> parse_connection(
        const YAML::Node& key_node, const YAML::Node& value_node);
};

}  // namespace sim
