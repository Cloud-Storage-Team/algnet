#include <gtest/gtest.h>

#include "scheduler/scheduler.hpp"
#include "utils.hpp"

namespace test {

TEST_F(TestScheduler, BigExpectedProcessingOrder) {
    std::size_t number_of_events = 5;

    TimeNs last_time = TimeNs(0);
    srand(31);
    std::uint32_t min_time = 1;
    uint32_t max_time = 3000;

    for (std::size_t i = 0; i < number_of_events; i++) {
        sim::Scheduler::get_instance().add(
            TimeNs(rand() % (max_time - min_time + 1)), [&]() {
                TimeNs cur_time =
                    sim::Scheduler::get_instance().get_current_time();
                EXPECT_GE(cur_time, last_time);
                last_time = cur_time;
                for (std::size_t i = 0; i < number_of_events; i++) {
                    TimeNs random_shift(rand() % max_time);
                    sim::Scheduler::get_instance().add(
                        cur_time + random_shift, [&]() {
                            TimeNs cur_time = sim::Scheduler::get_instance()
                                                  .get_current_time();
                            EXPECT_GE(cur_time, last_time);
                            last_time = cur_time;
                            TimeNs random_shift(rand() % max_time);
                            sim::Scheduler::get_instance().add(
                                cur_time + random_shift, [&]() {});
                        });
                }
            });
    }

    while (sim::Scheduler::get_instance().tick()) {
    }
}

}  // namespace test
