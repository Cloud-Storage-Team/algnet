#include <gtest/gtest.h>
#include <yaml-cpp/yaml.h>

#include "config_schema/schema_server.hpp"

namespace sim {
namespace test2 {

TEST(TestBasicTypes, BasicTypes) {
    std::filesystem::path current_dir =
        std::filesystem::path(__FILE__).parent_path();
    ConfigNodeWithPreset node =
        load_file_with_presets(current_dir / "basic_types.yml");
    std::filesystem::path schemas_dir = current_dir / "_schemas";
    ConfigSchema schema_node = load_file(schemas_dir / "basic_types.schema");
    SchemaServer schema_server(schemas_dir);
    ASSERT_NO_THROW(schema_server.validate(schema_node, node));
}

TEST(TestBasicTypes, WrongBasicTypes) {
    std::filesystem::path current_dir =
        std::filesystem::path(__FILE__).parent_path();
    ConfigNodeWithPreset node =
        load_file_with_presets(current_dir / "basic_types_wrong.yml");
    std::filesystem::path schemas_dir = current_dir / "_schemas";
    ConfigSchema schema_node = load_file(schemas_dir / "basic_types.schema");
    SchemaServer schema_server(schemas_dir);
    ASSERT_ANY_THROW(schema_server.validate(schema_node, node));
}

TEST(TestCustomType, CustomType) {
    std::filesystem::path current_dir =
        std::filesystem::path(__FILE__).parent_path();
    ConfigNodeWithPreset node =
        load_file_with_presets(current_dir / "custom_type.yml");
    std::filesystem::path schemas_dir = current_dir / "_schemas";
    ConfigSchema schema_node =
        load_file(schemas_dir / "test_with_custom_type.schema");
    SchemaServer schema_server(schemas_dir);
    ASSERT_NO_THROW(schema_server.validate(schema_node, node));
}

TEST(TestCustomType, WrongCustomType) {
    std::filesystem::path current_dir =
        std::filesystem::path(__FILE__).parent_path();
    ConfigNodeWithPreset node =
        load_file_with_presets(current_dir / "custom_type_wrong.yml");
    std::filesystem::path schemas_dir = current_dir / "_schemas";
    ConfigSchema schema_node =
        load_file(schemas_dir / "test_with_custom_type.schema");
    SchemaServer schema_server(schemas_dir);
    ASSERT_ANY_THROW(schema_server.validate(schema_node, node));
}

TEST(TestRootIsType, RootIsType) {
    std::filesystem::path current_dir =
        std::filesystem::path(__FILE__).parent_path();
    ConfigNodeWithPreset node =
        load_file_with_presets(current_dir / "root_is_type.yml");
    std::filesystem::path schemas_dir = current_dir / "_schemas";
    ConfigSchema schema_node = load_file(schemas_dir / "root_is_type.schema");
    SchemaServer schema_server(schemas_dir);
    ASSERT_NO_THROW(schema_server.validate(schema_node, node));
}

TEST(TestRootIsType, WrongRootIsType) {
    std::filesystem::path current_dir =
        std::filesystem::path(__FILE__).parent_path();
    ConfigNodeWithPreset node =
        load_file_with_presets(current_dir / "root_is_type_wrong.yml");
    std::filesystem::path schemas_dir = current_dir / "_schemas";
    ConfigSchema schema_node = load_file(schemas_dir / "root_is_type.schema");
    SchemaServer schema_server(schemas_dir);
    ASSERT_ANY_THROW(schema_server.validate(schema_node, node));
}

}  // namespace test2
}  // namespace sim