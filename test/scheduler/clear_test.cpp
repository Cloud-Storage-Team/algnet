#include <gtest/gtest.h>

#include "utils.hpp"

TEST_F(TestScheduler, ClearEmptyAtStart) {
    EXPECT_FALSE(sim::Scheduler::get_instance().tick());
}

TEST_F(TestScheduler, EmptyAfterClear) {
    sim::AddEvents<sim::EmptyEvent>(3);
    sim::Scheduler::get_instance().clear();

    EXPECT_FALSE(sim::Scheduler::get_instance().tick());
}

TEST_F(TestScheduler, ClearWhenEmpty) {
    sim::Scheduler::get_instance().clear();
    EXPECT_FALSE(sim::Scheduler::get_instance().tick());
}
