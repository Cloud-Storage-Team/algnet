#include <gtest/gtest.h>

#include "packet.hpp"
#include "utils.hpp"

namespace test {

TEST_F(LinkTest, PacketArrived) {
    std::shared_ptr<sim::IRoutingDevice> src =
        std::make_shared<DeviceMock>(DeviceMock());
    std::shared_ptr<sim::IRoutingDevice> dst =
        std::make_shared<DeviceMock>(DeviceMock());
    auto link = std::make_shared<sim::Link>("", src, dst);

    int NUMBER_OF_PACKETS = 10;
    std::vector<sim::Packet> packets;

    for (int i = 0; i < NUMBER_OF_PACKETS; i++) {
        packets.push_back(sim::Packet(sim::PacketType::DATA, i));
        link->process_arrival(packets.back());
    }

    for (int i = 0; i < NUMBER_OF_PACKETS; i++) {
        ASSERT_EQ(link->get_packet(), packets[i]);
    }

    ASSERT_FALSE(link->get_packet().has_value());
}

}  // namespace test
