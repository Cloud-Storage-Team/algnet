#pragma once

#include <memory>
#include <queue>

#include "types.hpp"
#include "event_comparator.hpp"

namespace sim {

// Scheduler is implemented as a Singleton class
// which provides a global access to a single instance
class SimpleScheduler {
public:
    SimpleScheduler() = default;

    template <typename TEvent, typename... Args>
    void add(Args&&... args) {
        static_assert(std::is_constructible_v<TEvent, Args&&...>,
                      "Event must be constructible from given args");
        static_assert(std::is_base_of_v<Event, TEvent>,
                      "TEvent must inherit from Event");

        m_events.emplace(std::make_unique<TEvent>(args...));
    }

    void clear();  // Clear all events
    bool tick();
    Time get_current_time();

private:
    std::priority_queue<std::unique_ptr<Event>,
                        std::vector<std::unique_ptr<Event>>, EventComparator>
        m_events;

    Time m_current_event_local_time;
};

}  // namespace sim
