#pragma once

#include <queue>

namespace sim {

class Event;

// Scheduler is implemented as a Singleton class
// which provides a global access to a single instance
class Scheduler {
public:
    // Static method to get the instance
    static Scheduler& get_instance() {
        static Scheduler instance;
        return instance;
    }

    void tick();
    void add(const Event& event);
    int peek_time() const;

    // Clear all events
    void clear();

private:
    // Private constructor to prevent instantiation
    Scheduler() {}
    // No copy constructor and assignment operators
    Scheduler(const Scheduler&) = delete;
    Scheduler& operator=(const Scheduler&) = delete;

    std::priority_queue<Event> m_events;
};

}  // namespace sim
