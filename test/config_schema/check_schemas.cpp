#include <gtest/gtest.h>
#include <yaml-cpp/yaml.h>

#include "config_schema/schema_server.hpp"

namespace sim {
namespace test2 {

class SchemaTest : public ::testing::Test {
protected:
    void SetUp() override {
        current_dir = std::filesystem::path(__FILE__).parent_path();
        schemas_dir = current_dir / "_schemas";
        schema_server = std::make_unique<SchemaServer>(schemas_dir);
    }

    ConfigSchema LoadSchema(const std::string& name) {
        return load_file(schemas_dir / name);
    }

    ConfigNodeWithPreset LoadConfigNodeWithPreset(const std::string& name) {
        return load_file_with_presets(current_dir / name);
    }

    std::filesystem::path current_dir;
    std::filesystem::path schemas_dir;
    std::unique_ptr<SchemaServer> schema_server;
};

TEST_F(SchemaTest, BasicTypes) {
    ConfigNodeWithPreset node =
        LoadConfigNodeWithPreset(current_dir / "basic_types.yml");
    ConfigSchema schema_node = LoadSchema("basic_types.schema");

    ASSERT_NO_THROW(schema_server->validate(schema_node, node));
}

TEST_F(SchemaTest, WrongBasicTypes) {
    ConfigNodeWithPreset node =
        LoadConfigNodeWithPreset(current_dir / "basic_types_wrong.yml");
    ConfigSchema schema_node = LoadSchema("basic_types.schema");

    ASSERT_ANY_THROW(schema_server->validate(schema_node, node));
}

TEST_F(SchemaTest, CustomType) {
    ConfigSchema schema = LoadSchema("nested_custom_type.schema");
    ConfigNodeWithPreset node = LoadConfigNodeWithPreset("custom_type.yml");

    ASSERT_NO_THROW(schema_server->validate(schema, node));
}

TEST_F(SchemaTest, WrongCustomType) {
    ConfigSchema schema = LoadSchema("nested_custom_type.schema");
    ConfigNodeWithPreset node =
        LoadConfigNodeWithPreset("custom_type_wrong.yml");

    ASSERT_ANY_THROW(schema_server->validate(schema, node));
}

TEST_F(SchemaTest, RootIsType) {
    ConfigSchema schema = LoadSchema("root_is_type.schema");
    ConfigNodeWithPreset node = LoadConfigNodeWithPreset("root_is_type.yml");

    ASSERT_NO_THROW(schema_server->validate(schema, node));
}

TEST_F(SchemaTest, WrongRootIsType) {
    ConfigSchema schema = LoadSchema("root_is_type.schema");
    ConfigNodeWithPreset node =
        LoadConfigNodeWithPreset("root_is_type_wrong.yml");

    ASSERT_ANY_THROW(schema_server->validate(schema, node));
}

TEST_F(SchemaTest, AbsouletePaths) {
    ConfigSchema schema = LoadSchema("test_paths.schema");
    std::string expected_path = schemas_dir / "nested_absolute_path.schema";
    std::string nested_schema =
        schema["absolute_path"]["_type"].value().as<std::string>().value();
    std::filesystem::path nested_schema_path =
        std::filesystem::path(nested_schema);
    std::filesystem::path sub_schema_path =
        nested_schema_path.is_absolute()
            ? schemas_dir / nested_schema_path.relative_path()
            : std::filesystem::path(__FILE__).parent_path() /
                  nested_schema_path;
    ASSERT_EQ(expected_path, sub_schema_path.string());
}

TEST_F(SchemaTest, RelativePaths) {
    ConfigSchema schema = LoadSchema("test_paths.schema");
    std::string expected_path = std::filesystem::path(__FILE__).parent_path() /
                                "test_relative_paths/relative_path.schema";
    std::string nested_schema =
        schema["relative_path"]["_type"].value().as<std::string>().value();
    std::filesystem::path nested_schema_path =
        std::filesystem::path(nested_schema);
    std::filesystem::path sub_schema_path =
        nested_schema_path.is_absolute()
            ? schemas_dir / nested_schema_path.relative_path()
            : std::filesystem::path(__FILE__).parent_path() /
                  nested_schema_path;
    ASSERT_EQ(expected_path, sub_schema_path.string());
}

}  // namespace test2
}  // namespace sim