#include <gtest/gtest.h>

#include "utils.hpp"

namespace test {

class UpdateTable : public testing::Test {
public:
    void TearDown() override{};
    void SetUp() override{};
};

TEST_F(UpdateTable, RouteIsPresent) {
    auto source = std::make_shared<sim::RoutingModule>(sim::RoutingModule());
    auto neighbour = std::make_shared<sim::RoutingModule>(sim::RoutingModule());
    auto dest = std::make_shared<sim::RoutingModule>(sim::RoutingModule());
    auto another_dest =
        std::make_shared<sim::RoutingModule>(sim::RoutingModule());

    auto link_neighbour =
        std::make_shared<TestLink>(TestLink(source, neighbour));

    EXPECT_FALSE(source->check_route(dest, link_neighbour));
    EXPECT_FALSE(source->check_route(another_dest, link_neighbour));

    source->update_routing_table(dest, link_neighbour);

    EXPECT_TRUE(source->check_route(dest, link_neighbour));
    EXPECT_FALSE(source->check_route(another_dest, link_neighbour));
}

}  // namespace test
