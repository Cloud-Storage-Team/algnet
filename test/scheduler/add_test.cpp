#include <gtest/gtest.h>

#include "scheduler/scheduler.hpp"
#include "utils.hpp"

namespace test {

TEST_F(TestScheduler, AddExpectedAmountOfElements) {
    std::size_t number_of_events = 5;

    std::size_t cnt_called = 0;
    for (std::size_t i = 0; i < number_of_events; i++) {
        sim::Scheduler::get_instance().add(TimeNs(i),
                                           [&cnt_called]() { cnt_called++; });
    }

    while (sim::Scheduler::get_instance().tick()) {
    }

    EXPECT_EQ(cnt_called, number_of_events);
}

}  // namespace test
