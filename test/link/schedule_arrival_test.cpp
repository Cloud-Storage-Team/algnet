#include <gtest/gtest.h>

#include "packet.hpp"
#include "scheduler.hpp"
#include "utils.hpp"

namespace test {

TEST_F(LinkTest, ScheduledCorrectly) {
    std::shared_ptr<sim::IRoutingDevice> src =
        std::make_shared<DeviceMock>(DeviceMock());
    std::shared_ptr<sim::IRoutingDevice> dst =
        std::make_shared<DeviceMock>(DeviceMock());
    auto link = sim::Link(src, dst, 10, 10);

    int NUMBER_OF_PACKETS = 10;
    for (int i = 0; i < NUMBER_OF_PACKETS; i++) {
        link.schedule_arrival(sim::Packet(sim::DATA, 100));
    }

    int cnt = 0;
    while (sim::Scheduler::get_instance().tick()) {
        cnt++;
    }
    ASSERT_EQ(cnt, NUMBER_OF_PACKETS);

    for (int i = 0; i < NUMBER_OF_PACKETS; i++) {
        ASSERT_TRUE(link.get_packet().has_value());
    }

    ASSERT_FALSE(link.get_packet().has_value());
}

}  // namespace test