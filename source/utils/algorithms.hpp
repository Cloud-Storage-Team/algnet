#pragma once

#include <memory>
#include <unordered_map>

#include "device/routing_module.hpp"

namespace sim {

using RoutingTable =
    std::unordered_map<std::shared_ptr<IRoutingDevice>, std::unordered_map<std::shared_ptr<ILink>, int>>;

// returns start device routing table
// Unlike standard BFS that processes nodes one by one, this processes all nodes at the current distance level together.
// So each iteration is a processing of all devices at a certain distance (wavefront)
RoutingTable bfs(std::shared_ptr<IRoutingDevice>& start_device);

}  // namespace sim
