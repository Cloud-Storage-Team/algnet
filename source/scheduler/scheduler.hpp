#pragma once

#include <deque>
#include <iostream>
#include <memory>
#include <queue>

#include "event/new_event.hpp"
#include "near_events_storage.hpp"
#include "types.hpp"

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

    template <typename TEvent, typename Func>
    requires std::constructible_from<NewEvent, TimeNs, Func&&>
    void add(TimeNs event_time, Func&& func) {
        std::cout << "Add event for time " << event_time << " at moment "
                  << m_current_event_local_time << std::endl;
        if (event_time < m_current_event_local_time) {
            throw std::runtime_error("Try to schedule event in the past!");
        }

        if (is_near_event(event_time)) {
            m_near_events.add(NewEvent(event_time, std::forward<Func>(func)));
        } else {
            m_far_events.emplace(event_time, std::forward<Func>(func));
        }
    }

    void clear();  // Clear all events
    bool tick();
    TimeNs get_current_time();

private:
    static constexpr inline TimeNs M_MAX_COUNTSORT_CAPACITY = TimeNs(2'000);

    // Private constructor, copy constructor & assignment to prevent
    // instantiation
    Scheduler() : m_current_event_local_time(TimeNs(0)) {}
    Scheduler(const Scheduler&) = default;
    Scheduler& operator=(const Scheduler&) = default;

    [[nodiscard]] inline bool is_near_event(TimeNs event_time) {
        return event_time <
               m_current_event_local_time + M_MAX_COUNTSORT_CAPACITY;
    }

    inline NewEvent& top_far_event() {
        return const_cast<NewEvent&>(m_far_events.top());
    }

    inline void correctify_state() {
        while (!m_far_events.empty()) {
            NewEvent& event = top_far_event();
            assert(event.time >= m_current_event_local_time);

            if (is_near_event(event.time)) {
                m_near_events.add(std::move(event));
                m_far_events.pop();
            } else {
                break;
            }
        }
    }

    NearEventsStorage<M_MAX_COUNTSORT_CAPACITY.value_nanoseconds()>
        m_near_events;

    std::priority_queue<NewEvent, std::vector<NewEvent>,
                        std::greater<NewEvent> >
        m_far_events;

    TimeNs m_current_event_local_time;
};

}  // namespace sim
