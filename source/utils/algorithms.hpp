#pragma once

#include <memory>
#include <unordered_map>

#include "device/routing_module.hpp"

namespace sim {

using RoutingTable =
    std::unordered_map<std::shared_ptr<IRoutingDevice>, std::unordered_map<std::shared_ptr<ILink>, int>>;

// Builds routing table using BFS algorithm starting from start_device,
// for each device stores the first link that leads to it from the start
RoutingTable bfs(std::shared_ptr<IRoutingDevice>& start_device);

}  // namespace sim
