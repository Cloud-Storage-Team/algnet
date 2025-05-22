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

    static Time last_time;

    virtual void operator()() final;
};

struct StopEvent : public sim::Event {
    StopEvent(std::uint32_t a_time);
    ~StopEvent() = default;
    virtual void operator()() final;
};

struct TestEvent {
    TestEvent(const EmptyEvent& e) : event(e) {}
    TestEvent(const CountingEvent& e) : event(e) {}
    TestEvent(const ComparatorEvent& e) : event(e) {}
    TestEvent(const StopEvent& e) : event(e) {}

    void operator()() {
        std::visit([&](auto real_event) { real_event(); }, event);
    }
    bool operator>(const TestEvent& other) const {
        return get_time() > other.get_time();
    }
    bool operator<(const TestEvent& other) const {
        return get_time() < other.get_time();
    }
    Time get_time() const {
        return std::visit(
            [&](auto real_event) { return real_event.get_time(); }, event);
    }

    std::variant<EmptyEvent, CountingEvent, ComparatorEvent, StopEvent> event;
};

using Scheduler = sim::SchedulerTemplate<TestEvent>;

template <typename T>
void AddEvents(int number, std::shared_ptr<Time> event_time = nullptr);
}  // namespace test
