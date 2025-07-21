#pragma once

#include <gtest/gtest.h>

#include <ctime>

#include "scheduler.hpp"

namespace test {

class TestScheduler : public testing::Test {
public:
    void TearDown() override { sim::Scheduler::get_instance().clear(); }
    void SetUp() override {};
};

struct EmptyEvent : public sim::Event {
    EmptyEvent(std::uint32_t a_time);
    ~EmptyEvent() = default;
    virtual void operator()() final;
};

struct CountingEvent : public sim::Event {
    CountingEvent(std::uint32_t a_time);
    ~CountingEvent() = default;

    static int cnt;

    virtual void operator()() final;
};

struct ComparatorEvent : public sim::Event {
    ComparatorEvent(std::uint32_t a_time);
    ~ComparatorEvent() = default;

    static TimeNs last_time;

    virtual void operator()() final;
};

template <typename T>
void AddEvents(int number, std::shared_ptr<TimeNs> event_time = nullptr) {
    static_assert(std::is_base_of<sim::Event, T>::value,
                  "T must inherit from Event");

    srand(static_cast<unsigned int>(time(0)));
    TimeNs min_time = 1;
    TimeNs max_time = static_cast<TimeNs>(1e9);

    while ((number--) > 0) {
        if (event_time == nullptr) {
            sim::Scheduler::get_instance().add<T>(
                rand() % (max_time - min_time + 1) + min_time);
        } else {
            sim::Scheduler::get_instance().add<T>(++(*event_time.get()));
        }
    }
}
}  // namespace test
