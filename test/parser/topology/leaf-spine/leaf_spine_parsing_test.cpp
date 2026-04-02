#include <gtest/gtest.h>

#include <filesystem>

#include "parser/topology/leaf_spine_topology_parser.hpp"

namespace sim {
namespace test2 {

TEST(LeafSpineTopologyParsing, CheckTopology) {
    std::filesystem::path topology_path =
        std::filesystem::path(__FILE__).parent_path() / "topology.yml";

    const ConfigNodeWithPreset config = load_file_with_presets(topology_path);

    LeafSpineTopologyContext ctx = parse_leaf_spine_topology_context(config);

    ASSERT_EQ(ctx.spine_switches.size(), 3);
    ASSERT_EQ(ctx.leaf_switches.size(), 6);
    ASSERT_EQ(ctx.hosts.size(), 6);

    std::size_t expected_links_count =
        2 * (ctx.leaf_switches.size() * ctx.spine_switches.size() +
             ctx.hosts.size());

    ASSERT_EQ(ctx.links.size(), expected_links_count);
}

}  // namespace test2
}  // namespace sim
