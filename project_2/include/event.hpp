#pragma once

#include <cstdint>
#include <queue>
#include <memory>
#include <iostream>
#include <functional>

struct EventComparator;

class Event {
public:
    explicit Event(std::uint64_t event_time, std::function<void(std::priority_queue<std::shared_ptr<Event>, std::vector<std::shared_ptr<Event>>, EventComparator>&)> action, std::uint32_t priority = 0);

    void perform_action(std::priority_queue<std::shared_ptr<Event>, std::vector<std::shared_ptr<Event>>, EventComparator>& events);
    bool operator<(const Event& other) const;

    std::uint64_t event_time;
    std::uint32_t priority;
    std::function<void(std::priority_queue<std::shared_ptr<Event>, std::vector<std::shared_ptr<Event>>, EventComparator>&)> action;
};

struct EventComparator {
    bool operator()(const std::shared_ptr<Event>& lhs, const std::shared_ptr<Event>& rhs) {
        return (lhs->event_time > rhs->event_time) || 
               (lhs->event_time == rhs->event_time && lhs->priority > rhs->priority);
    }
};
