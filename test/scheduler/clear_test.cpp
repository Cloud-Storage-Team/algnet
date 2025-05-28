#include <gtest/gtest.h>

#include "utils.hpp"

namespace test {

TEST_F(TestScheduler, ClearEmptyAtStart) {
    EXPECT_FALSE(Scheduler::get_instance().tick());
}

TEST_F(TestScheduler, EmptyAfterClear) {
    AddEvents<EmptyEvent>(3);
    Scheduler::get_instance().clear();

    EXPECT_FALSE(Scheduler::get_instance().tick());
}

TEST_F(TestScheduler, ClearWhenEmpty) {
    Scheduler::get_instance().clear();
    EXPECT_FALSE(Scheduler::get_instance().tick());
}

}  // namespace test
