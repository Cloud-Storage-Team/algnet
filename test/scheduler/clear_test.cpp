#include <gtest/gtest.h>

#include "scheduler/scheduler.hpp"
#include "utils.hpp"

namespace test {

TEST_F(TestScheduler, ClearEmptyAtStart) {
    EXPECT_FALSE(sim::Scheduler::get_instance().tick());
}

TEST_F(TestScheduler, EmptyAfterClear) {
    for (std::size_t i = 0; i < 3; i++) {
        sim::Scheduler::get_instance().add(TimeNs(i), []() {});
    }
    sim::Scheduler::get_instance().clear();

    EXPECT_FALSE(sim::Scheduler::get_instance().tick());
}

TEST_F(TestScheduler, ClearWhenEmpty) {
    sim::Scheduler::get_instance().clear();
    EXPECT_FALSE(sim::Scheduler::get_instance().tick());
}

}  // namespace test
