#include <gtest/gtest.h>

#include "utils.hpp"

namespace test {

TEST_F(TestScheduler, StopEventWorksCorrectly) {
    int number_of_events = 5;

    CountingEvent::cnt = 0;
    std::shared_ptr<std::uint32_t> event_time =
        std::make_shared<std::uint32_t>(1);
    AddEvents<CountingEvent>(number_of_events, event_time);
    AddEvents<sim::Stop>(1, event_time);
    AddEvents<CountingEvent>(number_of_events, event_time);

    while (sim::Scheduler::get_instance().tick()) {
    }

    EXPECT_EQ(CountingEvent::cnt, number_of_events);
}

}  // namespace test