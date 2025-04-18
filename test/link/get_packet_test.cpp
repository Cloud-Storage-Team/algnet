#include <gtest/gtest.h>

#include "packet.hpp"
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

}  // namespace test