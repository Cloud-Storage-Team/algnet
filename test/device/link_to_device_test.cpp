#include <gtest/gtest.h>

#include "topology/device/routing_module/routing_module.hpp"
#include "utils.hpp"

namespace test {

class LinkToDevice : public testing::Test {
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

TEST_F(LinkToDevice, NoLinkToDevice) {
    auto source = std::make_shared<TestDevice>();
    auto dest = std::make_shared<TestDevice>();
    auto link = std::make_shared<TestLink>(source, dest);
    dest->add_inlink(link);

    auto destPacket = packet_with_dest(dest);
    EXPECT_EQ(source->get_link_to_destination(destPacket), nullptr);
}

TEST_F(LinkToDevice, LinkIsPresent) {
    auto source = std::make_shared<TestDevice>("s1");
    auto neighbour = std::make_shared<TestDevice>("s2");
    auto dest = std::make_shared<TestDevice>("d1");
    auto another_dest = std::make_shared<TestDevice>("d2");

    auto link_neighbour =
        std::make_shared<TestLink>(TestLink(source, neighbour));
    source->update_routing_table(neighbour->get_id(), link_neighbour);
    auto link_dest = std::make_shared<TestLink>(TestLink(source, dest));
    source->update_routing_table(dest->get_id(), link_dest);

    EXPECT_EQ(source->get_link_to_destination(packet_with_dest(dest)),
              link_dest);
    EXPECT_EQ(source->get_link_to_destination(packet_with_dest(neighbour)),
              link_neighbour);
    EXPECT_EQ(source->get_link_to_destination(packet_with_dest(another_dest)),
              nullptr);
    source->update_routing_table(another_dest->get_id(), link_neighbour);

    EXPECT_EQ(source->get_link_to_destination(packet_with_dest(another_dest)),
              link_neighbour);
}

}  // namespace test
