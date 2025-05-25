#pragma once

#include <gtest/gtest.h>

#include <ctime>

#include "event.hpp"
#include "scheduler.hpp"

namespace test {

class TestScheduler : public testing::Test {
public:
    void TearDown() override { sim::Scheduler::get_instance().clear(); }
    void SetUp() override {};
};

struct EmptyEvent {
    EmptyEvent(std::uint32_t a_time);
    ~EmptyEvent() = default;
    void operator()();

    Time get_time() const;

private:
    Time m_time;
};

struct CountingEvent {
    CountingEvent(std::uint32_t a_time);
    ~CountingEvent() = default;

    static int cnt;

    void operator()();
    Time get_time() const;

private:
    Time m_time;
};

struct ComparatorEvent {
    ComparatorEvent(std::uint32_t a_time);
    ~ComparatorEvent() = default;

    static Time last_time;

    void operator()();
    Time get_time() const;

private:
    Time m_time;
};

struct StopEvent {
    StopEvent(std::uint32_t a_time);
    ~StopEvent() = default;
    void operator()();
    Time get_time() const;

private:
    Time m_time;
};

struct TestEvent {
    TestEvent(const EmptyEvent& e);
    TestEvent(const CountingEvent& e);
    TestEvent(const ComparatorEvent& e);
    TestEvent(const StopEvent& e);

    void operator()();
    bool operator>(const TestEvent& other) const;
    bool operator<(const TestEvent& other) const;
    Time get_time() const;

    std::variant<EmptyEvent, CountingEvent, ComparatorEvent, StopEvent> event;
};

using Scheduler = sim::SchedulerTemplate<TestEvent>;

template <typename T>
void AddEvents(int number, std::shared_ptr<Time> event_time = nullptr);
}  // namespace test
