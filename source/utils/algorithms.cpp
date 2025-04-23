#include "algorithms.hpp"

#include "link.hpp"

#include <queue>

namespace sim {

// Builds routing table using BFS algorithm starting from start_device,
// for each device stores the first link that leads to it from the start
RoutingTable bfs(std::shared_ptr<IRoutingDevice>& start_device) {
    RoutingTable routing_table;
    std::queue<std::shared_ptr<IRoutingDevice>> queue;
    std::set<std::shared_ptr<IRoutingDevice>> used;
    queue.push(start_device);

    while (!queue.empty()) {
        std::shared_ptr<IRoutingDevice> device = queue.front();
        queue.pop();
        if (used.contains(device)) {
            continue;
        }
        used.insert(device);
        std::set<std::shared_ptr<ILink>> outlinks = device->get_outlinks();
        for (std::shared_ptr<ILink> link : outlinks) {
            auto next_hop = link->get_to();
            auto curr_device = link->get_from();
            if (used.contains(next_hop)) {
                continue;
            }
            if (curr_device == start_device) {
                routing_table[next_hop] = link;
            } else if (!routing_table.contains(next_hop)) {
                routing_table[next_hop] = routing_table[curr_device];
            }
            queue.push(next_hop);
        }
    }
    return routing_table;
};

}  // namespace sim
