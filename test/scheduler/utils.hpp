#pragma once

#include <gtest/gtest.h>

#include <ctime>
#include <type_traits>

#include "../../source/event.hpp"

class TestScheduler : public testing::Test {
public:
    void TearDown() override{
        sim::Scheduler::get_instance().clear();
    };
    void SetUp() override{};
};

namespace sim {
struct EmptyEvent : public Event {
    ~EmptyEvent() = default;
    virtual void operator()() final{};
};

struct CountingEvent : public Event {
    ~CountingEvent() = default;

    static int cnt;

    virtual void operator()() final { cnt++; };
};

struct ComparatorEvent : public Event {
    ~ComparatorEvent() = default;

    static std::uint32_t last_time;

    virtual void operator()() final {
        EXPECT_GE(time, last_time);
        last_time = time;
    };
};

template <typename T>
void AddEvents(int number) {
    static_assert(std::is_base_of<Event, T>::value,
                  "T must inherit from Event");

    srand(time(0));
    std::uint32_t min_time = 1;
    std::uint32_t max_time = static_cast<std::uint32_t>(1e9);

    while ((number--) > 0) {
        std::unique_ptr<Event> event_ptr = std::make_unique<T>(T());
        event_ptr->time = rand() % (max_time - min_time + 1) + min_time;
        Scheduler::get_instance().add(std::move(event_ptr));
    }
}
}  // namespace sim
