#include <gtest/gtest.h>
#include <random> // Include for random number generation
#include <algorithm> // Include for std::shuffle

#include "utils.hpp"

namespace test {

class Neighbours : public testing::Test {
public:
    void TearDown() override{};
    void SetUp() override{};
};


TEST_F(Neighbours, NeighboursAreCalculatedCorrectly) {
    const size_t NUMBER_OF_NEIGHBOURS = 3;
    const size_t NUMBER_OF_DESTINATIONS = 5;

    std::random_device rd; 
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> neighbourDist(0, NUMBER_OF_NEIGHBOURS - 1);

    auto source = std::make_shared<sim::RoutingModule>(sim::RoutingModule());
    auto neighbours = createRoutingModules(NUMBER_OF_NEIGHBOURS);
    auto dests = createRoutingModules(NUMBER_OF_DESTINATIONS);
    
    auto targetNeighbours = std::vector<std::shared_ptr<TestLink>>();
    auto links = std::vector<std::shared_ptr<TestLink>>();
    for (int i = 0; i < NUMBER_OF_DESTINATIONS; i++) {
        size_t randomNeighbourIndex = neighbourDist(gen);
        links.emplace_back(std::make_shared<TestLink>(TestLink(source, neighbours[randomNeighbourIndex])));
        targetNeighbours.emplace_back(neighbours[randomNeighbourIndex]);
    }

    EXPECT_TRUE(source->get_neighbours().empty());

    std::shuffle(dests.begin(), dests.end(), gen);
    for (int i = 0; i < NUMBER_OF_DESTINATIONS; i++) {
        source->update_routing_table(dests[i], links[i]);
    }

    auto sourceNeighbours = source->get_neighbours();
    EXPECT_EQ(sourceNeighbours.size(), targetNeighbours.size());
    for (const auto& neighbour : sourceNeighbours) {
        EXPECT_TRUE(std::find(targetNeighbours.begin(), targetNeighbours.end(), neighbour) != targetNeighbours.end());
    }
}


// TEST_F(Neighbours, NeighboursAreCalculatedCorrectly) {
//     auto source = std::make_shared<TestRoutingModule>(TestRoutingModule());

//     size_t NUMBER_OF_NEIGHBOURS = 3;
//     auto neighbours = std::vector<std::shared_ptr<TestRoutingModule>>();
//     for (int i = 0; i < NUMBER_OF_NEIGHBOURS; i++) {
//         neighbours.emplace_back(TestRoutingModule());
//     }

//     size_t NUMBER_OF_DESTINATIONS = 5;
//     auto dests = std::vector<std::shared_ptr<TestRoutingModule>>();
//     for (int i = 0; i < NUMBER_OF_DESTINATIONS; i++) {
//         dests.emplace_back(TestRoutingModule());
//     }

//     auto link1_neighbour1 =
//         std::make_shared<TestLink>(TestLink(source, neighbours[0]));
//     auto link2_neighbour1 =
//         std::make_shared<TestLink>(TestLink(source, neighbours[0]));
//     auto link3_neighbour1 =
//         std::make_shared<TestLink>(TestLink(source, neighbours[0]));
//     auto link1_neighbour2 =
//         std::make_shared<TestLink>(TestLink(source, neighbours[1]));
//     auto link1_neighbour3 =
//         std::make_shared<TestLink>(TestLink(source, neighbours[2]));

//     EXPECT_TRUE(source->get_neighbours().empty());

//     source->update_routing_table(dests[0], link2_neighbour1);
//     source->update_routing_table(dests[1], link1_neighbour3);
//     source->update_routing_table(dests[2], link1_neighbour2);
//     source->update_routing_table(dests[3], link3_neighbour1);
//     source->update_routing_table(dests[4], link1_neighbour1);

//     std::vector<std::shared_ptr<sim::IRoutingDevice>> neighbours =
//         source->get_neighbours();
//     EXPECT_TRUE(neighbours.size() == 3);
//     for (auto neighbour : neighbours) {
//         EXPECT_TRUE(neighbour == neighbours[0] || neighbour == neighbours[1] ||
//                     neighbour == neighbours[2]);
//     }
// }

}  // namespace test
