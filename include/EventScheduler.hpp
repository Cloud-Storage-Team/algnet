#pragma once

#include "Event.hpp"

#include <queue>
#include <functional>

class EventScheduler {
public:
    EventScheduler() = default;

    void Schedule(double time, const std::function<void()>& handler);
    Event PopNextEvent();
    double PeekNextEventTime() const;
    bool Empty() const;

    std::priority_queue<Event> events;
};