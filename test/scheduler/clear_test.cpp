#include <gtest/gtest.h>

#include "scheduler.hpp"
#include "utils.hpp"

TEST(TestScheduler, ClearEmptyAtStart) {
    EXPECT_FALSE(sim::Scheduler::get_instance().tick());
}

TEST(TestScheduler, EmptyAfterClear) {
    sim::AddEvents<sim::EmptyEvent>(3);
    sim::Scheduler::get_instance().clear();

    EXPECT_FALSE(sim::Scheduler::get_instance().tick());
}

TEST(TestScheduler, ClearWhenEmpty) {
    sim::Scheduler::get_instance().clear();
    EXPECT_FALSE(sim::Scheduler::get_instance().tick());
}
