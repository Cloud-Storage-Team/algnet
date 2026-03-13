#include <gtest/gtest.h>

#include "utils.hpp"

namespace test {

TEST_F(LinkTest, ScheduledCorrectly) {
    std::shared_ptr<sim::IDevice> src =
        std::make_shared<DeviceMock>(DeviceMock());
    std::shared_ptr<sim::IDevice> dst =
        std::make_shared<DeviceMock>(DeviceMock());
    auto link =
        sim::Link::create_shared("", src, dst, SpeedGbps(10), TimeNs(10));

    int NUMBER_OF_PACKETS = 10;
    for (int i = 0; i < NUMBER_OF_PACKETS; i++) {
        link->schedule_arrival(sim::Packet(SizeByte(100)));
    }

    while (sim::Scheduler::get_instance().tick()) {
        ;
    }

    for (int i = 0; i < NUMBER_OF_PACKETS; i++) {
        ASSERT_TRUE(link->has_packet());
        ASSERT_NO_THROW(link->pop_packet());
    }

    ASSERT_FALSE(link->has_packet());
}

}  // namespace test
