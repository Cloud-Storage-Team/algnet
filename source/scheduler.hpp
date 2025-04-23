#pragma once

#include <memory>
#include <queue>

#include "event.hpp"

namespace sim {

struct EventComparator {
    bool operator()(const std::unique_ptr<Event>& lhs,
                    const std::unique_ptr<Event>& rhs) const {
        return (*lhs.get()) > (*rhs.get());
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

    void add(std::unique_ptr<Event> event);
    void clear();  // Clear all events
    bool tick();

private:
    // Private constructor to prevent instantiation
    Scheduler() {}
    // No copy constructor and assignment operators
    Scheduler(const Scheduler&) = delete;
    Scheduler& operator=(const Scheduler&) = delete;

    std::priority_queue<std::unique_ptr<Event>,
                        std::vector<std::unique_ptr<Event>>, EventComparator>
        m_events;
};

}  // namespace sim
