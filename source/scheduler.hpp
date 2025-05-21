#pragma once

#include <queue>

#include "event.hpp"

namespace sim {

struct EventComparator {
    bool operator()(const EventVariant& lhs, const EventVariant& rhs) const {
        return std::visit(
            [](const auto& a, const auto& b) -> bool { return a > b; }, lhs,
            rhs);
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

    void add(EventVariant event);
    void clear();  // Clear all events
    bool tick();

private:
    // Private constructor to prevent instantiation
    Scheduler() {}
    // No copy constructor and assignment operators
    Scheduler(const Scheduler&) = delete;
    Scheduler& operator=(const Scheduler&) = delete;

    std::priority_queue<EventVariant, std::vector<EventVariant>,
                        EventComparator>
        m_events;
};

}  // namespace sim
