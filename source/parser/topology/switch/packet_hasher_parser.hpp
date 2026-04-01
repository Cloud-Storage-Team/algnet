#pragma once
#include <memory>

#include "parser/config_reader/config_node.hpp"
#include "topology/device/hashers/i_hasher.hpp"
namespace sim {

std::unique_ptr<IPacketHasher> parse_packet_hasher(const ConfigNode& node,
                                                   const Id& switch_id);

}