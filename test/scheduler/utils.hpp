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

struct TestStopEvent : public sim::Event {
    TestStopEvent(std::uint32_t a_time);
    ~TestStopEvent() = default;
    virtual void operator()() final;
};

template <typename T, typename... Args>
struct concatenator;

template <typename... Args0, typename... Args1>
struct concatenator<std::variant<Args0...>, Args1...> {
    using type = std::variant<Args0..., Args1...>;
};

using TestEventVariant =
    concatenator<sim::EventVariant, EmptyEvent, CountingEvent, ComparatorEvent,
                 TestStopEvent>::type;

class Scheduler {
public:
    static Scheduler& get_instance() {
        static Scheduler instance;
        return instance;
    }

    void add(TestEventVariant event) { m_events.push(std::move(event)); }

    void clear() {
        m_events =
            std::priority_queue<TestEventVariant, std::vector<TestEventVariant>,
                                TestEventComparator>();
    }

    bool tick() {
        if (m_events.empty()) {
            return false;
        }

        TestEventVariant event = std::move(m_events.top());
        m_events.pop();
        m_current_event_local_time = std::visit(
            [](const auto& event) { return event.get_time(); }, event);
        std::visit([](auto& e) { e.operator()(); }, event);
        return true;
    }

    Time get_current_time() const { return m_current_event_local_time; }

private:
    struct TestEventComparator {
        bool operator()(const TestEventVariant& lhs,
                        const TestEventVariant& rhs) const {
            return std::visit(
                [](const auto& a, const auto& b) -> bool { return a > b; }, lhs,
                rhs);
        }
    };

    Scheduler() = default;
    Scheduler(const Scheduler&) = delete;
    Scheduler& operator=(const Scheduler&) = delete;

    std::priority_queue<TestEventVariant, std::vector<TestEventVariant>,
                        TestEventComparator>
        m_events;

    Time m_current_event_local_time = 0;
};

template <typename T>
void AddEvents(int number, std::shared_ptr<Time> event_time = nullptr);
}  // namespace test
