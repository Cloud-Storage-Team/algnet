#pragma once

#include <cstdint>
#include <queue>

#include "event.hpp"

namespace sim {

class EventComparator {
    bool operator()(const Event* a_first_event, const Event* a_second_event) {
        return a_first_event->get_time() > a_second_event->get_time();
    }
};

// Scheduler is implemented as a Singleton class
// which provides a global access to a single instance
class Scheduler {
public:
    // Static method to get the instance
    static Scheduler& get_instance() {
        static Scheduler instance;
        return instance;
    }

    void add(const Event* event);
    void clear();  // Clear all events
    void tick();

private:
    // Private constructor to prevent instantiation
    Scheduler() {}
    // No copy constructor and assignment operators
    Scheduler(const Scheduler&) = delete;
    Scheduler& operator=(const Scheduler&) = delete;

    std::priority_queue<Event*, std::vector<Event*>, EventComparator> m_events;
};

}  // namespace sim
