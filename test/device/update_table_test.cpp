#include <gtest/gtest.h>

#include "utils.hpp"

namespace {

class UpdateTable : public testing::Test {
public:
    void TearDown() override{};
    void SetUp() override{};
};

TEST_F(UpdateTable, RoutPresents) {
    sim::TestDevice* source = new sim::TestDevice();
    sim::TestDevice* neighbour = new sim::TestDevice();
    sim::TestDevice* dest = new sim::TestDevice();
    sim::TestDevice* another_dest = new sim::TestDevice();

    sim::TestLink* link_neighbour = new sim::TestLink(source, neighbour);

    EXPECT_FALSE(source->check_rout(dest, link_neighbour));
    EXPECT_FALSE(source->check_rout(another_dest, link_neighbour));

    source->update_routing_table(dest, link_neighbour);

    EXPECT_TRUE(source->check_rout(dest, link_neighbour));
    EXPECT_FALSE(source->check_rout(another_dest, link_neighbour));
}

}  // namespace
