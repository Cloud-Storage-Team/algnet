#pragma once
#include "config_reader/config_node.hpp"
#include "simulator/network/network.hpp"

namespace sim {

Network parse_network(const ConfigNode& node);

}
