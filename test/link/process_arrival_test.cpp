#include <gtest/gtest.h>

#include "../../source/packet.hpp"
#include "utils.hpp"

namespace test {

TEST_F(LinkTest, PacketArrived) {
    std::shared_ptr<sim::IRoutingDevice> src =
        std::make_shared<DeviceMock>(DeviceMock());
    std::shared_ptr<sim::IRoutingDevice> dst =
        std::make_shared<DeviceMock>(DeviceMock());
    auto link = sim::Link(src, dst);

    int AMOUNT_OF_PACKETS = 10;
    for (int i = 0; i < AMOUNT_OF_PACKETS; i++) {
        link.process_arrival(sim::Packet());
    }

    for (int i = 0; i < AMOUNT_OF_PACKETS; i++) {
        ASSERT_TRUE(link.get_packet().has_value());
    }

    ASSERT_FALSE(link.get_packet().has_value());
}

}  // namespace test