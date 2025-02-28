#include <gtest/gtest.h>

#include "../../source/scheduler.hpp"
#include "utils.hpp"

TEST_F(TestScheduler, TickReturnsFalseOnStop) {
    // TODO: implement after discussing mechanism   
    EXPECT_TRUE(false);

    sim::Scheduler::get_instance().clear();
}