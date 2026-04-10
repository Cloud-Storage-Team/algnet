#pragma once
#include <filesystem>

#include "network/network.hpp"
#include "parser/config_reader/config_node.hpp"

namespace sim {

Network parse_custom_network(const Topology& topology, const ConfigNode& node);

}
