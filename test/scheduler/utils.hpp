#pragma once

#include <gtest/gtest.h>

#include <ctime>
#include <type_traits>

#include "event.hpp"
#include "scheduler.hpp"

namespace test {

class TestScheduler : public testing::Test {
public:
    void TearDown() override { sim::Scheduler::get_instance().clear(); }
    void SetUp() override {};
};

struct EmptyEvent : public sim::Event {
    ~EmptyEvent() = default;
    virtual void operator()() final;
};

struct CountingEvent : public sim::Event {
    ~CountingEvent() = default;

    static int cnt;

    virtual void operator()() final;
};

struct ComparatorEvent : public sim::Event {
    ~ComparatorEvent() = default;

    static std::uint32_t last_time;

    virtual void operator()() final;
};

template <typename T>
void AddEvents(int number, std::shared_ptr<std::uint32_t> event_time = nullptr);
}  // namespace test
