#include <gtest/gtest.h>

#include <filesystem>

#include "../check_connectivity.hpp"
#include "parser/topology/fat_tree_topology_parser.hpp"

namespace sim {
namespace test2 {

TEST(FaTreeTopologyParsing, CheckTopology) {
    std::filesystem::path topology_path =
        std::filesystem::path(__FILE__).parent_path() / "topology.yml";

    const ConfigNodeWithPreset config = load_file_with_presets(topology_path);

    FatTreeTopologyContext ctx = parse_fat_tree_topology_context(config);

    // switch ports number
    std::size_t k = 4;
    // see
    // https://packetpushers.net/blog/demystifying-dcn-topologies-clos-fat-trees-part2/#:~:text=Here%20are%20some%20characteristics%20of%20Fat%2Dtrees:%20*,total%20of%2032768%20(8192%20x%204)%20servers
    std::size_t core_switches_count = k * k / 4;
    std::size_t aggr_switches_count = k * k / 2;
    std::size_t edge_switches_count = k * k / 2;
    std::size_t hosts_count = edge_switches_count * k / 2;

    ASSERT_EQ(ctx.switches.core.size(), core_switches_count);
    ASSERT_EQ(ctx.switches.aggr.size(), aggr_switches_count);
    ASSERT_EQ(ctx.switches.edge.size(), edge_switches_count);
    ASSERT_EQ(ctx.hosts.size(), hosts_count);

    std::size_t core_aggr_links_count = aggr_switches_count * k / 2;
    std::size_t aggr_edge_links_count = aggr_switches_count * k / 2;
    std::size_t edge_host_links_count = edge_switches_count * k / 2;

    // * 2 becase of each link stores twice: fowrard and backward
    ASSERT_EQ(ctx.links.core_aggr.size(), core_aggr_links_count * 2);
    ASSERT_EQ(ctx.links.aggr_edge.size(), aggr_edge_links_count * 2);
    ASSERT_EQ(ctx.links.edge_host.size(), edge_host_links_count * 2);

    check_connectivity(Topology(ctx.to_topology_context()));
}

}  // namespace test2
}  // namespace sim
