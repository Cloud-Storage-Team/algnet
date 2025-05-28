#include <gtest/gtest.h>

#include "utils.hpp"

namespace test {

TEST_F(TestScheduler, StopEventWorksCorrectly) {
    int number_of_events = 5;

    CountingEvent::cnt = 0;
    std::shared_ptr<Time> event_time =
        std::make_shared<Time>(1);
    AddEvents<CountingEvent>(number_of_events, event_time);
    AddEvents<sim::Stop>(1, event_time);
    AddEvents<CountingEvent>(number_of_events, event_time);

    while (sim::Scheduler::get_instance().tick()) {
    }

    EXPECT_EQ(CountingEvent::cnt, number_of_events);
}

}  // namespace test
