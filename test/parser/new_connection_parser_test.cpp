#include <gtest/gtest.h>

#include "../_mocks/host_gmock.hpp"
#include "parser/config_reader/config_node_with_preset.hpp"
#include "parser/network/new-connection/connection_parser.hpp"

namespace sim {

// TODO: replace all namespace test {...} with namespace sim {namespace test
// {...} } & replace 'test2' with 'test' here
namespace test2 {

TEST(ConnectionParser, SimpleConnection) {
    std::string config_name = "simple-connection.yaml";
    std::filesystem::path config_path(
        std::filesystem::path(__FILE__).parent_path() / config_name);

    const ConfigNode config = load_file(config_path);

    std::optional<ConfigNode> presets_node =
        config["connection-presets"].to_optional();

    std::string_view connection_name = "test-connection";

    auto exp_connection_node = config[connection_name];

    EXPECT_TRUE(exp_connection_node.has_value())
        << "Config does not have section '" << connection_name << "'";

    const ConfigNode connection_node = exp_connection_node.value();

    ConfigNodeWithPreset node(connection_node, presets_node);

    utils::IdTable<IHost> hosts_table;

    hosts_table["sender0"] = std::make_shared<test::HostGMock>();
    hosts_table["receiver0"] = std::make_shared<test::HostGMock>();

    EXPECT_NO_THROW(parse_i_connection(node, hosts_table));
}
}  // namespace test2

}  // namespace sim
