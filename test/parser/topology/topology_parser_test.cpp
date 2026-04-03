#include "parser/topology/topology_parser.hpp"

#include <gtest/gtest.h>

#include <filesystem>

#include "check_connectivity.hpp"

namespace sim {
namespace test2 {

TEST(TopologyParsing, BusTopology) {
    std::filesystem::path bus_topology_path =
        std::filesystem::path(__FILE__).parent_path() / "bus_topology.yml";

    const ConfigNodeWithPreset config =
        load_file_with_presets(bus_topology_path);

    Topology top = parse_topology(config);

    const TopologyContext& ctx = top.get_context();

    ASSERT_EQ(ctx.hosts_table.size(), 4);
    ASSERT_EQ(ctx.switches_table.size(), 1);
    ASSERT_EQ(ctx.links_table.size(), 8);
    check_connectivity(top);
}

}  // namespace test2
}  // namespace sim
