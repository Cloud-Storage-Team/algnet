#include <gtest/gtest.h>

#include "utils.hpp"

namespace test {

class Neighbours : public testing::Test {
public:
    void TearDown() override{};
    void SetUp() override{};
};

TEST_F(Neighbours, NeighboursAreCalculatedCorrectly) {
    auto source = std::make_shared<TestRoutingModule>(TestRoutingModule());
    std::cout << "Create s" << std::endl;

    auto neighbour1 = std::make_shared<TestRoutingModule>(TestRoutingModule());
    auto neighbour2 = std::make_shared<TestRoutingModule>(TestRoutingModule());
    auto neighbour3 = std::make_shared<TestRoutingModule>(TestRoutingModule());
    std::cout << "Create n" << std::endl;

    auto dest1 = std::make_shared<TestRoutingModule>(TestRoutingModule());
    auto dest2 = std::make_shared<TestRoutingModule>(TestRoutingModule());
    auto dest3 = std::make_shared<TestRoutingModule>(TestRoutingModule());
    auto dest4 = std::make_shared<TestRoutingModule>(TestRoutingModule());
    auto dest5 = std::make_shared<TestRoutingModule>(TestRoutingModule());
    std::cout << "Create d" << std::endl;

    auto link1_neighbour1 =
        std::make_shared<TestLink>(TestLink(source, neighbour1));
    auto link2_neighbour1 =
        std::make_shared<TestLink>(TestLink(source, neighbour1));
    auto link3_neighbour1 =
        std::make_shared<TestLink>(TestLink(source, neighbour1));
    auto link1_neighbour2 =
        std::make_shared<TestLink>(TestLink(source, neighbour2));
    auto link1_neighbour3 =
        std::make_shared<TestLink>(TestLink(source, neighbour3));
    std::cout << "Create ls" << std::endl;

    EXPECT_TRUE(source->get_neighbours().empty());
    std::cout << "Check empty" << std::endl;

    source->update_routing_table(dest1, link2_neighbour1);
    source->update_routing_table(dest2, link1_neighbour3);
    source->update_routing_table(dest3, link1_neighbour2);
    source->update_routing_table(dest4, link3_neighbour1);
    source->update_routing_table(dest5, link1_neighbour1);

    std::vector<std::shared_ptr<sim::IRoutingDevice>> neighbours =
        source->get_neighbours();
    EXPECT_TRUE(neighbours.size() == 3);
    for (auto neighbour : neighbours) {
        EXPECT_TRUE(neighbour == neighbour1 || neighbour == neighbour2 ||
                    neighbour == neighbour3);
    }
}

}  // namespace test
