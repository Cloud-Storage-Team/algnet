#pragma once
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include "event.hpp"
#include "types.hpp"

namespace sim {

// Scheduler is implemented as a Singleton class
// which provides a global access to a single instance
class MultithreadScheduler {
public:
    MultithreadScheduler();

    template <typename TEvent, typename... Args>
    void add(Args&&... args) {
        static_assert(std::is_constructible_v<TEvent, Args&&...>,
                      "Event must be constructible from given args");
        static_assert(std::is_base_of_v<Event, TEvent>,
                      "TEvent must inherit from Event");

        m_events.emplace_back(std::make_unique<TEvent>(args...));
        if (m_events.back()->get_time() <
            m_events[m_earliest_event_index]->get_time()) {
            m_earliest_event_index = m_events.size() - 1;
        }
    }

    void clear();
    bool tick();
    Time get_current_time() const;

private:
    std::mutex m_events_mutex;
    std::vector<std::unique_ptr<Event> > m_events;

    size_t m_earliest_event_index;
    Time m_current_event_local_time;
};

}  // namespace sim