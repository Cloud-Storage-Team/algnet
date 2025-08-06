#pragma once
#include <spdlog/fmt/fmt.h>
#include <yaml-cpp/yaml.h>

#include <memory>

#include "connection/connection.hpp"

namespace sim {

class ConnectionParser {
public:
    static std::shared_ptr<Connection> parse_connection(const YAML::Node& key_node,
                                                 const YAML::Node& value_node);
};

}  // namespace sim
