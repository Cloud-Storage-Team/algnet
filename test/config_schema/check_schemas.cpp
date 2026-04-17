#include <gtest/gtest.h>
#include <yaml-cpp/yaml.h>

#include "config_schema/validator.hpp"

namespace sim {
namespace test2 {

TEST(TestBasicTypes, BasicTypes) {
    std::filesystem::path current_dir =
        std::filesystem::path(__FILE__).parent_path();
    ConfigNodeWithPreset node =
        load_file_with_presets(current_dir / "basic_types.yml");
    YAML::Node schema_node =
        YAML::LoadFile(current_dir.append("basic_types.schema"));
    ASSERT_NO_THROW(validate(schema_node, node));
}

TEST(TestBasicTypes, WrongBasicTypes) {
    std::filesystem::path current_dir =
        std::filesystem::path(__FILE__).parent_path();
    ConfigNodeWithPreset node =
        load_file_with_presets(current_dir / "basic_types_wrong.yml");
    YAML::Node schema_node =
        YAML::LoadFile(current_dir.append("basic_types.schema"));
    ASSERT_ANY_THROW(validate(schema_node, node));
}

}  // namespace test2
}  // namespace sim