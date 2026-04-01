#include <gtest/gtest.h>

#include "scheduler/scheduler.hpp"
#include "utils.hpp"

namespace test {

TEST_F(TestScheduler, ExpectedProcessingOrder) {
    std::size_t number_of_events = 5;

    TimeNs last_time = TimeNs(0);
    srand(31);
    std::uint32_t min_time = 1;
    uint32_t max_time = 1e9;

    for (std::size_t i = 0; i < number_of_events; i++) {
        sim::Scheduler::get_instance().add(
            TimeNs(rand() % (max_time - min_time + 1)), [&]() {
                TimeNs cur_time =
                    sim::Scheduler::get_instance().get_current_time();
                EXPECT_GE(cur_time, last_time);
                last_time = cur_time;
            });
    }

    while (sim::Scheduler::get_instance().tick()) {
    }
}

}  // namespace test
