#pragma once

#include <cppcoro/task.hpp>
#include <memory>
#include <queue>

#include "event_comparator.hpp"
#include "types.hpp"
#include "utils/spinlock.hpp"

namespace sim {

// Scheduler is implemented as a Singleton class
// which provides a global access to a single instance
class AsyncScheduler {
public:
    AsyncScheduler() = default;

    template <typename TEvent, typename... Args>
    void add(Args&&... args) {
        static_assert(std::is_constructible_v<TEvent, Args&&...>,
                      "Event must be constructible from given args");
        static_assert(std::is_base_of_v<Event, TEvent>,
                      "TEvent must inherit from Event");

        m_events.emplace(std::make_unique<TEvent>(std::forward<Args>(args)...));
    }

    void clear();  // Clear all events
    bool tick();
    Time get_current_time();

private:
    cppcoro::task<> pop();
    cppcoro::task<> execute_event(std::unique_ptr<Event> event);

    SpinLock m_lock;  // lock for m_events
    std::priority_queue<std::unique_ptr<Event>,
                        std::vector<std::unique_ptr<Event>>, EventComparator>
        m_events;

    Time m_current_event_local_time;
};

}  // namespace sim
