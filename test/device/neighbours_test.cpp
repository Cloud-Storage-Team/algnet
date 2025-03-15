#include <gtest/gtest.h>

#include <algorithm>  // Include for std::shuffle
#include <random>     // Include for random number generation

#include "utils.hpp"

namespace test {

class Neighbours : public testing::Test {
public:
    void TearDown() override {};
    void SetUp() override {};
};

TEST_F(Neighbours, NeighboursAreCalculatedCorrectly) {
    auto source = std::make_shared<sim::RoutingModule>(sim::RoutingModule());

    size_t NUMBER_OF_NEIGHBOURS = 3;
    auto target_neighbours = createRoutingModules(NUMBER_OF_NEIGHBOURS);
    size_t NUMBER_OF_DESTINATIONS = 5;
    auto dests = createRoutingModules(NUMBER_OF_DESTINATIONS);

    auto link1_neighbour1 =
        std::make_shared<TestLink>(TestLink(source, target_neighbours[0]));
    auto link2_neighbour1 =
        std::make_shared<TestLink>(TestLink(source, target_neighbours[0]));
    auto link3_neighbour1 =
        std::make_shared<TestLink>(TestLink(source, target_neighbours[0]));
    auto link1_neighbour2 =
        std::make_shared<TestLink>(TestLink(source, target_neighbours[1]));
    auto link1_neighbour3 =
        std::make_shared<TestLink>(TestLink(source, target_neighbours[2]));

    EXPECT_TRUE(source->get_neighbours().empty());

    source->update_routing_table(dests[0], link2_neighbour1);
    source->update_routing_table(dests[1], link1_neighbour3);
    source->update_routing_table(dests[2], link1_neighbour2);
    source->update_routing_table(dests[3], link3_neighbour1);
    source->update_routing_table(dests[4], link1_neighbour1);

    std::vector<std::shared_ptr<sim::IRoutingDevice>> neighbours =
        source->get_neighbours();
    EXPECT_TRUE(neighbours.size() == 3);
    for (auto neighbour : neighbours) {
        EXPECT_TRUE(neighbour == target_neighbours[0] ||
                    neighbour == target_neighbours[1] ||
                    neighbour == target_neighbours[2]);
    }
}

}  // namespace test
