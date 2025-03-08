#include <gtest/gtest.h>

#include "../../source/packet.hpp"
#include "utils.hpp"

namespace test {

TEST_F(LinkTest, NoPacketToGet) {
    std::shared_ptr<sim::IRoutingDevice> src =
        std::make_shared<DeviceMock>(DeviceMock());
    std::shared_ptr<sim::IRoutingDevice> dst =
        std::make_shared<DeviceMock>(DeviceMock());
    auto link = sim::Link(src, dst);

    ASSERT_FALSE(link.get_packet().has_value());
}

TEST_F(LinkTest, SimpleGetPacket) {
    std::shared_ptr<sim::IRoutingDevice> src =
        std::make_shared<DeviceMock>(DeviceMock());
    std::shared_ptr<sim::IRoutingDevice> dst =
        std::make_shared<DeviceMock>(DeviceMock());
    auto link = sim::Link(src, dst);

    sim::Packet packet;
    link.process_arrival(packet);

    ASSERT_EQ(link.get_packet().value(), packet);
}

TEST_F(LinkTest, FIFO) {
    std::shared_ptr<sim::IRoutingDevice> src =
        std::make_shared<DeviceMock>(DeviceMock());
    std::shared_ptr<sim::IRoutingDevice> dst =
        std::make_shared<DeviceMock>(DeviceMock());
    auto link = sim::Link(src, dst);

    int AMOUNT_OF_PACKETS = 10;
    std::vector<sim::Packet> packets;

    for (int i = 0; i < AMOUNT_OF_PACKETS; i++) {
        packets.push_back(sim::Packet(sim::PacketType::DATA, i));
        link.process_arrival(packets.back());
    }

    for (int i = 0; i < AMOUNT_OF_PACKETS; i++) {
        ASSERT_EQ(link.get_packet(), packets[i]);
    }
}

}  // namespace test