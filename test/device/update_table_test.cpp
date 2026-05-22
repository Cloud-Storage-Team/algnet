#include <gtest/gtest.h>

#include "utils.hpp"

namespace test {

class UpdateTable : public testing::Test {
public:
    void TearDown() override {};
    void SetUp() override {};
};

static sim::PacketPtr packet_with_dest(
    std::shared_ptr<sim::Identifiable> device) {
    sim::PacketPtr packet = std::make_shared<sim::Packet>();
    packet->receiver_id = device->get_id();
    return packet;
}

TEST_F(UpdateTable, RouteIsPresent) {
    auto source = std::make_shared<TestDevice>("s1");
    auto neighbour = std::make_shared<TestDevice>("m1");
    auto dest = std::make_shared<TestDevice>("d1");
    auto another_dest = std::make_shared<TestDevice>("d2");

    auto link_neighbour =
        std::make_shared<TestLink>(TestLink(source, neighbour));

    EXPECT_EQ(source->get_link_to_destination(packet_with_dest(dest)), nullptr);
    EXPECT_EQ(source->get_link_to_destination(packet_with_dest(another_dest)),
              nullptr);

    source->update_routing_table(dest->get_id(), link_neighbour);

    EXPECT_EQ(source->get_link_to_destination(packet_with_dest(dest)),
              link_neighbour);
    EXPECT_EQ(source->get_link_to_destination(packet_with_dest(another_dest)),
              nullptr);
}

}  // namespace test
