#pragma once

#include <functional>
#include <queue>

#include "event.hpp"

namespace sim {

// Scheduler is implemented as a Singleton class
// which provides a global access to a single instance
template <typename TEvent>
requires requires(const TEvent& e) {
    { e.get_time() } -> std::same_as<Time>;
}
class SchedulerTemplate {
    static_assert(
        std::is_constructible_v<bool, decltype(std::declval<TEvent>() >
                                               std::declval<TEvent>())>,
        "Event must have operator >");
    static_assert(std::is_invocable_v<TEvent>, "Event must be invocable");

public:
    // Static method to get the instance
    static SchedulerTemplate& get_instance() {
        static SchedulerTemplate instance;
        return instance;
    }

    void add(const TEvent& event) { m_events.emplace(event); }

    // Clear all events
    void clear() {
        std::priority_queue<TEvent, std::vector<TEvent>, std::greater<TEvent>>().swap(m_events);
    }

    bool tick() {
        if (m_events.empty()) {
            return false;
        }

        TEvent event = m_events.top();
        m_events.pop();
        m_current_event_local_time = event.get_time(); 
        event.operator()();
        return true;
    }

    Time get_current_time() { return m_current_event_local_time; }

private:
    // Private constructor to prevent instantiation
    SchedulerTemplate() : m_events(), m_current_event_local_time(0) {}
    // No copy constructor and assignment operators
    SchedulerTemplate(const SchedulerTemplate&) = delete;
    SchedulerTemplate& operator=(const SchedulerTemplate&) = delete;

    std::priority_queue<TEvent, std::vector<TEvent>, std::greater<TEvent>> m_events;

    Time m_current_event_local_time;
};

using Scheduler = SchedulerTemplate<BaseEvent>;

}  // namespace sim
