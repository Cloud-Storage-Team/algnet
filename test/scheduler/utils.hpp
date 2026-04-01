#pragma once

#include <gtest/gtest.h>

#include <ctime>

#include "scheduler/scheduler.hpp"

namespace test {

class TestScheduler : public testing::Test {
public:
    void TearDown() override { sim::Scheduler::get_instance().clear(); }
    void SetUp() override { sim::Scheduler::get_instance().clear(); };
};

}  // namespace test
