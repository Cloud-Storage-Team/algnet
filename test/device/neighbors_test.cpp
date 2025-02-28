#include <gtest/gtest.h>

#include "utils.hpp"

namespace {

class Neighbors : public testing::Test {
public:
    void TearDown() override{};
    void SetUp() override{};
};

TEST(Neighbors, TwoPointersToTheSameDevice) {
    sim::TestDevice* source = new sim::TestDevice();

    sim::TestDevice* neighbour1 = new sim::TestDevice();
    sim::TestDevice* neighbour2 = new sim::TestDevice();
    sim::TestDevice* neighbour3 = new sim::TestDevice();

    sim::TestDevice* dest1 = new sim::TestDevice();
    sim::TestDevice* dest2 = new sim::TestDevice();
    sim::TestDevice* dest3 = new sim::TestDevice();
    sim::TestDevice* dest4 = new sim::TestDevice();
    sim::TestDevice* dest5 = new sim::TestDevice();

    sim::TestLink* link1_neighbour1 = new sim::TestLink(source, neighbour1);
    sim::TestLink* link2_neighbour1 = new sim::TestLink(source, neighbour1);
    sim::TestLink* link3_neighbour1 = new sim::TestLink(source, neighbour1);
    sim::TestLink* link1_neighbour2 = new sim::TestLink(source, neighbour2);
    sim::TestLink* link1_neighbour3 = new sim::TestLink(source, neighbour3);

    EXPECT_TRUE(source->get_neighbors().empty());

    source->update_routing_table(dest1, link2_neighbour1);
    source->update_routing_table(dest2, link1_neighbour3);
    source->update_routing_table(dest3, link1_neighbour2);
    source->update_routing_table(dest4, link3_neighbour1);
    source->update_routing_table(dest5, link1_neighbour1);

    std::vector<sim::Device*> neighbors = source->get_neighbors();
    EXPECT_TRUE(neighbors.size() == 3);
    for (auto* neighbour : neighbors) {
        EXPECT_TRUE(neighbour == neighbour1 || neighbour == neighbour2 ||
                    neighbour == neighbour3);
    }
}

}  // namespace
