#include <gtest/gtest.h>

#include <filesystem>

#include "parser/topology/topology_parser.hpp"

namespace sim {
namespace test2 {

TEST(IncastTopologyParsing, CustomBottleneck) {
    std::filesystem::path bus_topology_path =
        std::filesystem::path(__FILE__).parent_path() /
        "incast_custom_bottleneck.yml";

    const ConfigNodeWithPreset config =
        load_file_with_presets(bus_topology_path);

    Topology top = parse_topology(config);

    const TopologyContext& ctx = top.get_context();

    const std::size_t senders_count = 5;
    const std::size_t receivers_count = 1;
    const std::size_t hosts_count = senders_count + receivers_count;
    const std::size_t switches_count = 1;

    // 2 links between each host & switch
    const std::size_t links_count = (hosts_count) * 2;

    ASSERT_EQ(ctx.hosts_table.size(), hosts_count);
    ASSERT_EQ(ctx.switches_table.size(), switches_count);
    ASSERT_EQ(ctx.links_table.size(), links_count);

    std::map<LinkContext, std::size_t> link_context_table;
    for (auto [_, link] : ctx.links_table) {
        link_context_table[link->get_ctx()]++;
    }
    ASSERT_EQ(link_context_table.size(), 2)
        << "Custom topology should contain two different types of links";
    std::vector<std::size_t> eq_links_counts;
    for (auto [_, count] : link_context_table) {
        eq_links_counts.emplace_back(count);
    }
    sort(eq_links_counts.begin(), eq_links_counts.end());
    ASSERT_EQ(eq_links_counts, std::vector<std::size_t>(
                                   {receivers_count * 2, senders_count * 2}));
}

TEST(IncastTopologyParsing, CommonBottleneck) {
    std::filesystem::path bus_topology_path =
        std::filesystem::path(__FILE__).parent_path() /
        "incast_common_bottleneck.yml";

    const ConfigNodeWithPreset config =
        load_file_with_presets(bus_topology_path);

    Topology top = parse_topology(config);

    const TopologyContext& ctx = top.get_context();

    const std::size_t senders_count = 5;
    const std::size_t receivers_count = 1;
    const std::size_t hosts_count = senders_count + receivers_count;
    const std::size_t switches_count = 1;

    // 2 links between each host & switch
    const std::size_t links_count = (hosts_count) * 2;

    ASSERT_EQ(ctx.hosts_table.size(), hosts_count);
    ASSERT_EQ(ctx.switches_table.size(), switches_count);
    ASSERT_EQ(ctx.links_table.size(), links_count);

    std::map<LinkContext, std::size_t> link_context_table;
    for (auto [_, link] : ctx.links_table) {
        link_context_table[link->get_ctx()]++;
    }
    ASSERT_EQ(link_context_table.size(), 1)
        << "Topology should contain only one type of links";
}

}  // namespace test2
}  // namespace sim
