#pragma once

#include <cstdint>
#include <queue>

#include "event.hpp"

namespace sim {

// Scheduler is implemented as a Singleton class
// which provides a global access to a single instance
class Scheduler {
public:
    // Static method to get the instance
    static Scheduler& get_instance() {
        static Scheduler instance;
        return instance;
    }

    // TODO: remove implementation and use existing from .cpp
    void add(const Event& event) {};
    void clear() {};  // Clear all events
    void tick();

private:
    // Private constructor to prevent instantiation
    Scheduler() {}
    // No copy constructor and assignment operators
    Scheduler(const Scheduler&) = delete;
    Scheduler& operator=(const Scheduler&) = delete;

    std::priority_queue<Event> m_events;
};

}  // namespace sim
