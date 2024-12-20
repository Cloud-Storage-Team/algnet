#pragma once

#include "Event.hpp"

#include <queue>
#include <functional>

class EventScheduler {
public:
    EventScheduler() = default;

    void Schedule(std::uint64_t time, const std::function<void()>& handler);
    Event PopNextEvent();
    std::uint64_t PeekNextEventTime() const;
    bool Empty() const;

    std::priority_queue<Event> events;
};