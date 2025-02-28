#include <gtest/gtest.h>

#include "utils.hpp"

namespace {

class LinkToDevice : public testing::Test {
public:
    void TearDown() override{};
    void SetUp() override{};
};

TEST_F(LinkToDevice, NoLinkToDEvice) {
    sim::TestDevice* source = new sim::TestDevice();
    sim::TestDevice* dest = new sim::TestDevice();

    EXPECT_EQ(dest->get_link_to_device(dest), nullptr);
}

TEST_F(LinkToDevice, LinkPresents) {
    sim::TestDevice* source = new sim::TestDevice();
    sim::TestDevice* neighbour = new sim::TestDevice();
    sim::TestDevice* dest = new sim::TestDevice();
    sim::TestDevice* another_dest = new sim::TestDevice();

    sim::TestLink* link_neighbour = new sim::TestLink(source, neighbour);
    source->update_routing_table(neighbour, link_neighbour);
    sim::TestLink* link_dest = new sim::TestLink(source, dest);
    source->update_routing_table(dest, link_dest);

    EXPECT_EQ(source->get_link_to_device(dest), link_dest);
    EXPECT_EQ(source->get_link_to_device(neighbour), link_neighbour);
    EXPECT_EQ(source->get_link_to_device(another_dest), nullptr);
    source->update_routing_table(another_dest, link_neighbour);

    EXPECT_EQ(source->get_link_to_device(another_dest), link_neighbour);
}

}  // namespace
