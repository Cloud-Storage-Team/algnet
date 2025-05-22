#include <gtest/gtest.h>

#include <ctime>

#include "../link/utils.hpp"
#include "../switch/link_mock.hpp"
#include "event.hpp"
#include "scheduler.hpp"

namespace test {

class TestScheduler : public testing::Test {
public:
    void TearDown() override { sim::Scheduler::get_instance().clear(); }
    void SetUp() override {};
};

using Event = sim::Arrive;

std::shared_ptr<DeviceMock> device = std::make_shared<DeviceMock>();
std::shared_ptr<LinkMock> link = std::make_shared<LinkMock>(device, device);

TEST(TestScheduler, AddAndExecuteSingleEvent) {
    sim::Scheduler& scheduler = sim::Scheduler::get_instance();
    scheduler.clear();

    constexpr int event_time = 10;
    scheduler.add(Event(event_time, link, sim::Packet{}));

    ASSERT_TRUE(scheduler.tick());
    ASSERT_EQ(scheduler.get_current_time(), event_time);
    ASSERT_FALSE(scheduler.tick());
}

TEST(TestScheduler, ExecuteStopEvent) {
    sim::Scheduler& scheduler = sim::Scheduler::get_instance();
    scheduler.clear();

    constexpr int event1_time = 10;
    constexpr int event2_time = 20;
    constexpr int event3_time = 30;
    constexpr int event4_time = 100;
    constexpr int stop_time = 40;

    scheduler.add(Event(event1_time, link, sim::Packet{}));
    scheduler.add(Event(event2_time, link, sim::Packet{}));
    scheduler.add(Event(event3_time, link, sim::Packet{}));
    scheduler.add(sim::Stop(stop_time));
    scheduler.add(Event(event4_time, link, sim::Packet{}));

    ASSERT_TRUE(scheduler.tick());
    ASSERT_EQ(scheduler.get_current_time(), event1_time);
    ASSERT_TRUE(scheduler.tick());
    ASSERT_EQ(scheduler.get_current_time(), event2_time);
    ASSERT_TRUE(scheduler.tick());
    ASSERT_EQ(scheduler.get_current_time(), event3_time);

    ASSERT_TRUE(scheduler.tick());
    ASSERT_EQ(scheduler.get_current_time(), stop_time);

    ASSERT_FALSE(scheduler.tick());
}

}  // namespace test
