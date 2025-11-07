#pragma once

#include <unordered_map>

#include "device/interfaces/i_routing_device.hpp"
#include "link/i_link.hpp"

namespace sim {

using RoutingTable = std::unordered_map<Id, MapWeakPtr<ILink, int>>;

// returns start device routing table
// Unlike standard BFS that processes nodes one by one, this processes all nodes
// at the current distance level together. So each iteration is a processing of
// all devices at a certain distance (wavefront)
RoutingTable bfs(std::shared_ptr<IRoutingDevice> start_device);

}  // namespace sim
