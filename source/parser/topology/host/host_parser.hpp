#pragma once
#include <spdlog/fmt/fmt.h>

#include <memory>

#include "device/host.hpp"
#include "parser/config_reader/config_node.hpp"

namespace sim {

class HostParser {
public:
    static std::shared_ptr<IHost> parse_i_host(const YAML::Node& key_node,
                                               const YAML::Node& value_node);

private:
    static std::shared_ptr<Host> parse_default_host(
        const YAML::Node& key_node, const YAML::Node& value_node);
};

}  // namespace sim
