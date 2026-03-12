#include "parser/network/network_parser.hpp"

#include <gtest/gtest.h>

namespace sim {
namespace test2 {

TEST(NetworkParsing, BusNetwork) {
    std::filesystem::path config_path =
        std::filesystem::path(__FILE__).parent_path() / "bus_network.yml";

    Network network = parse_network(config_path);

    const NetworkContext& ctx = network.get_context();

    ASSERT_EQ(ctx.connections_table.size(), 4);
}

}  // namespace test2
}  // namespace sim
