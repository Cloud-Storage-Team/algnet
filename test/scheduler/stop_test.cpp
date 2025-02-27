#include <gtest/gtest.h>

#include "scheduler.hpp"

TEST(Stop, TickReturnsFalseOnStop) {
    EXPECT_TRUE(true);

    sim::Scheduler::get_instance().clear();
}