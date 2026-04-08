#include <gtest/gtest.h>

#include "parser/config_reader/config_node.hpp"
#include "parser/config_reader/config_node_with_preset.hpp"

namespace sim {
namespace test2 {

TEST(PathConfigNode, NoPath) {
    YAML::Node root = YAML::Load(R"(
a:
  b:
    c: 1
)");
    ConfigNode node(root, std::nullopt);
    auto a = node["a"].value();
    auto b = a["b"].value();
    auto c = b["c"].value();

    ASSERT_EQ(b.get_config_path(), std::nullopt);
    ASSERT_EQ(c.get_config_path(), std::nullopt);
}

TEST(PathConfigNode, PathExists) {
    std::filesystem::path check_path =
        std::filesystem::path(__FILE__).parent_path() / "check_paths.yml";
    ConfigNode node = load_file(check_path);
    auto result = node["connections"]["connection-1->1"].value();
    printf("path: %s\n", check_path);
    printf("result path: %s\n", result.get_config_path());
    fflush(stdout);
    ASSERT_EQ(result.get_config_path(), check_path);
}

TEST(PathConfigNodeWithPreset, NoPath) {
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

    ASSERT_EQ(b.get_config_path(), "root");
    ASSERT_EQ(c.get_config_path(), "root");
}

TEST(PathConfigNodeWithPreset, PathExists) {
    std::filesystem::path bus_topology_path =
        std::filesystem::path(__FILE__).parent_path() / "check_paths.yml";
    ConfigNodeWithPreset node = load_file_with_presets(bus_topology_path);
    auto result = node["connections"]["connection-1->1"].value();

    ASSERT_EQ(result.get_config_path(), bus_topology_path);
}

TEST(PathConfigNode, IterateMap) {
    std::filesystem::path check_path =
        std::filesystem::path(__FILE__).parent_path() / "iterate_map.yml";

    ConfigNode node = load_file(check_path);
    auto map = node["map"].value();
    for (auto it : map) {
        ASSERT_EQ(it.get_config_path(), check_path);
    }
}

TEST(PathConfigNode, IterateList) {
    std::filesystem::path check_path =
        std::filesystem::path(__FILE__).parent_path() / "iterate_list.yml";

    ConfigNode node = load_file(check_path);
    auto list = node["list"].value();
    for (auto it : list) {
        ASSERT_EQ(it.get_config_path(), check_path);
    }
}

}  // namespace test2
}  // namespace sim