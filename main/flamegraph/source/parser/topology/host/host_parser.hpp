#pragma once
#include <spdlog/fmt/fmt.h>

#include <memory>

#include "device/host.hpp"
#include "parser/config_reader/config_node.hpp"

namespace sim {

class HostParser {
public:
    static std::shared_ptr<IHost> parse_i_host(const ConfigNode& host_node);

private:
    static std::shared_ptr<Host> parse_default_host(
        const ConfigNode& host_node);
};

}  // namespace sim
