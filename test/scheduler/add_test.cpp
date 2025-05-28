#include <gtest/gtest.h>

#include "utils.hpp"

namespace test {

TEST_F(TestScheduler, AddExpectedAmountOfElements) {
    int number_of_events = 5;

    CountingEvent::cnt = 0;
    AddEvents<CountingEvent>(number_of_events);

    while (Scheduler::get_instance().tick()) {
    }

    EXPECT_EQ(CountingEvent::cnt, number_of_events);
}

}  // namespace test
