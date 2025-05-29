#include <gtest/gtest.h>

#include "utils.hpp"
#include "../utils/fake_packet.hpp"

namespace test {

class UpdateTable : public testing::Test {
public:
    void TearDown() override {};
    void SetUp() override {};
};

TEST_F(UpdateTable, RouteIsPresent) {
    auto source = std::make_shared<sim::RoutingModule>();
    auto neighbour = std::make_shared<sim::RoutingModule>();
    auto dest = std::make_shared<sim::RoutingModule>();
    auto another_dest = std::make_shared<sim::RoutingModule>();

    auto link_neighbour =
        std::make_shared<TestLink>(TestLink(source, neighbour));

    EXPECT_EQ(source->get_link_to_destination(FakePacket(dest)), nullptr);
    EXPECT_EQ(source->get_link_to_destination(FakePacket(another_dest)), nullptr);

    source->update_routing_table(dest->get_id(), link_neighbour);

    EXPECT_EQ(source->get_link_to_destination(FakePacket(dest)), link_neighbour);
    EXPECT_EQ(source->get_link_to_destination(FakePacket(another_dest)), nullptr);
}

}  // namespace test
