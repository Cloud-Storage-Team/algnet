#include <gtest/gtest.h>
#include <yaml-cpp/yaml.h>

#include "config_schema/validator.hpp"

namespace sim {
namespace test2 {

TEST(TestBasicTypes, BasicTypes) {
    ConfigNodeWithPreset node = load_file_with_presets("basic_types.yml");
    YAML::Node schema_node = YAML::LoadFile("basic_types.schema");
    validate(schema_node, node);
    ASSERT_NO_THROW(validate(schema_node, node));
}

TEST(TestBasicTypes, WrongBasicTypes) {
    ConfigNodeWithPreset node = load_file_with_presets("basic_types_wrong.yml");
    YAML::Node schema_node = YAML::LoadFile("basic_types.schema");
    ASSERT_ANY_THROW(validate(schema_node, node));
}

}  // namespace test2
}  // namespace sim