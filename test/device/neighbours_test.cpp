#include <gtest/gtest.h>

#include "utils.hpp"

namespace test {

class Neighbours : public testing::Test {
public:
    void TearDown() override{};
    void SetUp() override{};
};

TEST_F(Neighbours, NeighboursAreCalculatedCorrectly) {
    TestDevice source = TestDevice();

    TestDevice neighbour1 = TestDevice();
    TestDevice neighbour2 = TestDevice();
    TestDevice neighbour3 = TestDevice();

    TestDevice dest1 = TestDevice();
    TestDevice dest2 = TestDevice();
    TestDevice dest3 = TestDevice();
    TestDevice dest4 = TestDevice();
    TestDevice dest5 = TestDevice();

    TestLink link1_neighbour1 = TestLink(&source, &neighbour1);
    TestLink link2_neighbour1 = TestLink(&source, &neighbour1);
    TestLink link3_neighbour1 = TestLink(&source, &neighbour1);
    TestLink link1_neighbour2 = TestLink(&source, &neighbour2);
    TestLink link1_neighbour3 = TestLink(&source, &neighbour3);

    EXPECT_TRUE(source.get_neighbors().empty());

    source.update_routing_table(&dest1, &link2_neighbour1);
    source.update_routing_table(&dest2, &link1_neighbour3);
    source.update_routing_table(&dest3, &link1_neighbour2);
    source.update_routing_table(&dest4, &link3_neighbour1);
    source.update_routing_table(&dest5, &link1_neighbour1);

    std::vector<sim::Device*> neighbours = source.get_neighbors();
    EXPECT_TRUE(neighbours.size() == 3);
    for (auto* neighbour : neighbours) {
        EXPECT_TRUE(neighbour == &neighbour1 || neighbour == &neighbour2 ||
                    neighbour == &neighbour3);
    }
}

}  // namespace
