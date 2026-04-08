#include <gtest/gtest.h>

#include "parser/config_reader/config_node.hpp"
#include "parser/config_reader/config_node_with_preset.hpp"

namespace sim {
namespace test2 {

TEST(PathConfigNode, InsertedPath) {
    YAML::Node root = YAML::Load(R"(
a:
  b:
    c: 1
)");
    ConfigNode node(root, std::nullopt);
    auto a = node["a"].value();
    auto b = a["b"].value();
    auto c = b["c"].value();

    ASSERT_EQ(b.get_path_node(), std::nullopt);
    ASSERT_EQ(c.get_path_node(), std::nullopt);
}

TEST(PathConfigNode, MissingKey) {
    std::filesystem::path bus_topology_path =
        std::filesystem::path(__FILE__).parent_path() / "bus_network.yml";
    ConfigNode node = load_file(bus_topology_path);
    auto result = node["connections"]["connection-1->1"].value();

    ASSERT_EQ(result.get_path_node(), bus_topology_path.string());
}

TEST(PathConfigNodeWithPreset, InsertedPath) {
    YAML::Node root = YAML::Load(R"(
a:
  b:
    c: 1
)");
    ConfigNode tmp(root, std::nullopt, "root");
    ConfigNodeWithPreset node(tmp);
    auto a = node["a"].value();
    auto b = a["b"].value();
    auto c = b["c"].value();

    ASSERT_EQ(b.get_path_node(), "root");
    ASSERT_EQ(c.get_path_node(), "root");
}

TEST(PathConfigNodeWithPreset, MissingKey) {
    std::filesystem::path bus_topology_path =
        std::filesystem::path(__FILE__).parent_path() / "bus_network.yml";
    ConfigNodeWithPreset node = load_file_with_presets(bus_topology_path);
    auto result = node["connections"]["connection-1->1"].value();

    ASSERT_EQ(result.get_path_node(), bus_topology_path.string());
}

}  // namespace test2
}  // namespace sim