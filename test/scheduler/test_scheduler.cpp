#include <gtest/gtest.h>

#include <ctime>

#include "../switch/link_mock.hpp"
#include "../link/utils.hpp"
#include "event.hpp"
#include "scheduler.hpp"

namespace test {

class TestScheduler : public testing::Test {
public:
    void TearDown() override { sim::Scheduler::get_instance().clear(); }
    void SetUp() override {};
};

using Event = sim::Arrive;

TEST(TestScheduler, AddAndExecuteSingleEvent) {
    sim::Scheduler& scheduler = sim::Scheduler::get_instance();
    scheduler.clear();

    constexpr int event_time = 10;
    auto device = std::make_shared<DeviceMock>();
    auto link = std::make_shared<LinkMock>(device, device);
    scheduler.add(Event(event_time, link, sim::Packet{}));

    ASSERT_TRUE(scheduler.tick());
    ASSERT_EQ(scheduler.get_current_time(), event_time);
    ASSERT_FALSE(scheduler.tick());
}


}  // namespace test
