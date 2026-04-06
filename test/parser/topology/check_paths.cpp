#pragma once

#include <gtest/gtest.h>

#include "parser/config_reader/config_node_with_preset.hpp"
#include "parser/config_reader/config_node.hpp"

namespace sim{
namespace test2{

TEST(PathConfigNode, InsertedPath){
    YAML::Node root = YAML::Load(R"(
a:
  b:
    c: 1
)");
    ConfigNode node(root, std::nullopt, "root");
    auto a = node["a"].value();
    auto b = node["b"].value();
    auto c = node["c"].value();
    ASSERT_EQ(a.get_path_node(), "root\\a");
    ASSERT_EQ(b.get_path_node(), "root\\a\\b");
    ASSERT_EQ(c.get_path_node(), "root\\a\\b\\c");
}

TEST(PathConfigNode, MissingKey){
    std::filesystem::path bus_topology_path =
        std::filesystem::path(__FILE__).parent_path() /
        "bus_network.yml";
    ConfigNode node = load_file(path);
    auto result = node["connection-1->1"].value();
    ASSERT_EQ(result.get_path_node(), bus_topology_path.string() + "connections\\connection-1->1");
}

}
} // namespace sim