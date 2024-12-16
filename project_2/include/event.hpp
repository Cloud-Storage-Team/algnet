#pragma once

#include <cstdint>
#include <queue>
#include <memory>
#include <iostream>

struct EventComparator;

class Event {
public:
    explicit Event(std::uint64_t event_time, std::uint32_t priority = 0);
    virtual ~Event() {};

    virtual void perform_action(std::priority_queue<std::shared_ptr<Event>, std::vector<std::shared_ptr<Event>>, EventComparator>& events) = 0;

    bool operator<(const Event& other) const;

    std::uint64_t event_time;
    std::uint32_t priority;
};

struct EventComparator {
    bool operator()(const std::shared_ptr<Event>& lhs, const std::shared_ptr<Event>& rhs) {
        return (lhs->event_time > rhs->event_time) || 
               (lhs->event_time == rhs->event_time && lhs->priority > rhs->priority);
    }
};