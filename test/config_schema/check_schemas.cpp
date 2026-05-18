#include <gtest/gtest.h>
#include <yaml-cpp/yaml.h>

#include "config_schema/schema_server.hpp"

namespace sim {
namespace test2 {

class SchemaTest : public ::testing::Test {
protected:
    void SetUp() override {
        current_dir = std::filesystem::path(__FILE__).parent_path();
        config_nodes_dir = current_dir / "config_nodes";
        schemas_dir = current_dir / "_schemas";
        schema_server = std::make_unique<SchemaServer>(schemas_dir);
    }

    ConfigSchema load_schema(const std::string& name) {
        return load_file(schemas_dir / name);
    }

    ConfigNodeWithPreset load_config_node_with_preset(const std::string& name) {
        return load_file_with_presets(config_nodes_dir / name);
    }

    std::filesystem::path current_dir;
    std::filesystem::path config_nodes_dir;
    std::filesystem::path schemas_dir;
    std::unique_ptr<SchemaServer> schema_server;
};

TEST_F(SchemaTest, BasicTypes) {
    ConfigNodeWithPreset node = load_config_node_with_preset("basic_types.yml");
    ConfigSchema schema_node = load_schema("basic_types.schema");

    ASSERT_NO_THROW(schema_server->validate(schema_node, node));
}

TEST_F(SchemaTest, WrongBasicTypes) {
    ConfigNodeWithPreset node =
        load_config_node_with_preset("basic_types_wrong.yml");
    ConfigSchema schema_node = load_schema("basic_types.schema");

    ASSERT_ANY_THROW(schema_server->validate(schema_node, node));
}

TEST_F(SchemaTest, CustomType) {
    ConfigSchema schema = load_schema("nested_custom_type.schema");
    ConfigNodeWithPreset node = load_config_node_with_preset("custom_type.yml");

    ASSERT_NO_THROW(schema_server->validate(schema, node));
}

TEST_F(SchemaTest, WrongCustomType) {
    ConfigSchema schema = load_schema("nested_custom_type.schema");
    ConfigNodeWithPreset node =
        load_config_node_with_preset("custom_type_wrong.yml");

    ASSERT_ANY_THROW(schema_server->validate(schema, node));
}

TEST_F(SchemaTest, RootIsType) {
    ConfigSchema schema = load_schema("root_is_type.schema");
    ConfigNodeWithPreset node =
        load_config_node_with_preset("root_is_type.yml");

    ASSERT_NO_THROW(schema_server->validate(schema, node));
}

TEST_F(SchemaTest, WrongRootIsType) {
    ConfigSchema schema = load_schema("root_is_type.schema");
    ConfigNodeWithPreset node =
        load_config_node_with_preset("root_is_type_wrong.yml");

    ASSERT_ANY_THROW(schema_server->validate(schema, node));
}

TEST_F(SchemaTest, CheckPaths) {
    ConfigSchema schema = load_schema("test_paths/test_paths.schema");
    ConfigNodeWithPreset config_node =
        load_config_node_with_preset("test_paths.yml");
    ASSERT_NO_THROW(schema_server->validate(schema, config_node));
}

}  // namespace test2
}  // namespace sim