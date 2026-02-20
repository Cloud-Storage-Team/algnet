#include "parser/topology/topology_parser.hpp"

#include <gtest/gtest.h>

#include <filesystem>

namespace sim {
namespace test2 {

TEST(TopologyParsing, BusTopology) {
    std::filesystem::path bus_topology_path =
        std::filesystem::path(__FILE__).parent_path() / "bus_topology.yml";

    const ConfigNode config = load_file(bus_topology_path);

    Topology top = parse_topology(config);

    const TopologyContext& ctx = top.get_context();

    ASSERT_EQ(ctx.hosts_table.size(), 4);
    ASSERT_EQ(ctx.switches_table.size(), 1);
    ASSERT_EQ(ctx.links_table.size(), 8);
}

}  // namespace test2
}  // namespace sim
