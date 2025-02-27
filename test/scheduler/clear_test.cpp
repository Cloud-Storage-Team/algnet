#include <gtest/gtest.h>

#include "scheduler.hpp"
#include "utils.hpp"

TEST(Clear, EmptyAtStart) {
    EXPECT_FALSE(sim::Scheduler::get_instance().tick());
}

TEST(Clear, EmptyAfterClear) {
    sim::AddEvents<sim::EmptyEvent>(3);
    sim::Scheduler::get_instance().clear();

    EXPECT_FALSE(sim::Scheduler::get_instance().tick());
}

TEST(Clear, ClearWhenEmpty) {
    sim::Scheduler::get_instance().clear();
    EXPECT_FALSE(sim::Scheduler::get_instance().tick());
}
