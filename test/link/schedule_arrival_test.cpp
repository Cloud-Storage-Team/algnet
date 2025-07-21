#include <gtest/gtest.h>

#include "utils.hpp"

namespace test {

TEST_F(LinkTest, ScheduledCorrectly) {
    std::shared_ptr<sim::IDevice> src =
        std::make_shared<DeviceMock>(DeviceMock());
    std::shared_ptr<sim::IDevice> dst =
        std::make_shared<DeviceMock>(DeviceMock());
    auto link = std::make_shared<sim::Link>("", src, dst, 10, 10);

    int NUMBER_OF_PACKETS = 10;
    for (int i = 0; i < NUMBER_OF_PACKETS; i++) {
        link->schedule_arrival(sim::Packet(SizeByte(100)));
    }

    while (sim::Scheduler::get_instance().tick()) {
        ;
    }

    for (int i = 0; i < NUMBER_OF_PACKETS; i++) {
        ASSERT_TRUE(link->get_packet().has_value());
    }

    ASSERT_FALSE(link->get_packet().has_value());
}

}  // namespace test
