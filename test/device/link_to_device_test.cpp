#include <gtest/gtest.h>

#include "utils.hpp"

namespace test {

class LinkToDevice : public testing::Test {
public:
    void TearDown() override {};
    void SetUp() override {};
};

TEST_F(LinkToDevice, NoLinkToDevice) {
    auto source = std::make_shared<sim::RoutingModule>(sim::RoutingModule(""));
    auto dest = std::make_shared<sim::RoutingModule>(sim::RoutingModule(""));
    auto link = std::make_shared<TestLink>(source, dest);
    dest->add_inlink(link);

    EXPECT_EQ(source->get_link_to_destination(dest), nullptr);
}

TEST_F(LinkToDevice, LinkIsPresent) {
    auto source = std::make_shared<sim::RoutingModule>(sim::RoutingModule(""));
    auto neighbour =
        std::make_shared<sim::RoutingModule>(sim::RoutingModule(""));
    auto dest = std::make_shared<sim::RoutingModule>(sim::RoutingModule(""));
    auto another_dest =
        std::make_shared<sim::RoutingModule>(sim::RoutingModule(""));

    auto link_neighbour =
        std::make_shared<TestLink>(TestLink(source, neighbour));
    source->update_routing_table(neighbour, link_neighbour);
    auto link_dest = std::make_shared<TestLink>(TestLink(source, dest));
    source->update_routing_table(dest, link_dest);

    EXPECT_EQ(source->get_link_to_destination(dest), link_dest);
    EXPECT_EQ(source->get_link_to_destination(neighbour), link_neighbour);
    EXPECT_EQ(source->get_link_to_destination(another_dest), nullptr);
    source->update_routing_table(another_dest, link_neighbour);

    EXPECT_EQ(source->get_link_to_destination(another_dest), link_neighbour);
}

}  // namespace test
