#include "utils/algorithms.hpp"

#include <queue>

namespace sim {

// returns start device routing table
// Unlike standard BFS that processes nodes one by one, this processes all nodes
// at the current distance level together. So each iteration is a processing of
// all devices at a certain distance (wavefront)
RoutingTable bfs(std::shared_ptr<IRoutingDevice> start_device) {
    RoutingTable routing_table;

    std::queue<std::shared_ptr<IRoutingDevice>> queue;
    // All devices with known distance
    std::set<std::shared_ptr<IRoutingDevice>> used;
    // Current wavefront
    std::set<std::shared_ptr<IRoutingDevice>> wave_front;
    wave_front.insert(start_device);
    while (!wave_front.empty()) {
        for (auto device : wave_front) {
            queue.push(device);
            used.insert(device);
        }
        wave_front.clear();

        while (!queue.empty()) {
            std::shared_ptr<IRoutingDevice> device = queue.front();
            queue.pop();

            std::set<std::shared_ptr<ILink>> outlinks = device->get_outlinks();
            for (std::shared_ptr<ILink> link : outlinks) {
                auto next_hop = link->get_to();
                auto curr_device = link->get_from();
                if (used.contains(next_hop)) {
                    continue;
                }
                wave_front.insert(next_hop);

                if (curr_device == start_device) {
                    routing_table[next_hop->get_id()][link] = 1;
                } else {
                    // Here we get all ways to get to the previous device and
                    // add them to next hop This part might be called several
                    // times for same device
                    for (auto [link, paths_count] :
                         routing_table[curr_device->get_id()]) {
                        routing_table[next_hop->get_id()][link] += paths_count;
                    }
                }
            }
        }
    }

    return routing_table;
};

}  // namespace sim
