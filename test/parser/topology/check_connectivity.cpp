#include <gtest/gtest.h>

#include "check_connectivity.hpp"
#include "utils/algorithms.hpp"

namespace sim {
namespace test2 {

void check_connectivity(const Topology& topology) {
    const TopologyContext& ctx = topology.get_context();
    std::vector<std::shared_ptr<IRoutingDevice> > devices;
    {
        for (const auto& [_, host] : ctx.hosts_table) {
            devices.emplace_back(host);
        }

        for (const auto& [_, swtch] : ctx.switches_table) {
            devices.emplace_back(swtch);
        }
    }

    for (const auto& start_device : devices) {
        RoutingTable table = bfs(start_device);
        const Id& start_id = start_device->get_id();

        for (const auto& end_device : devices) {
            const Id& end_id = end_device->get_id();
            if (end_id == start_id) {
                continue;
            }
            auto it = table.find(end_id);
            ASSERT_TRUE(it != table.end()) << fmt::format(
                "Routing table of device '{}' should cotnain record about host "
                "'{}'",
                start_id, end_id);

            ASSERT_FALSE(it->second.empty()) << fmt::format(
                "No links routes from device '{}' to '{}'", start_id, end_id);
        }
    }
}

}  // namespace test2
}  // namespace sim