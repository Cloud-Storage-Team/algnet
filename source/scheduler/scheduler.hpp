#pragma once

#include <deque>
#include <memory>
#include <queue>

#include "event/event.hpp"
#include "near_events_storage.hpp"
#include "types.hpp"

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

    template <typename TEvent, typename... Args>
    void add(Args&&... args) {
        static_assert(std::is_constructible_v<TEvent, Args&&...>,
                      "Event must be constructible from given args");
        static_assert(std::is_base_of_v<Event, TEvent>,
                      "TEvent must inherit from Event");

        std::unique_ptr<Event> event(
            std::make_unique<TEvent>(std::forward<Args>(args)...));

        TimeNs event_time = event->get_time();

        if (event_time < m_current_event_local_time) {
            throw std::runtime_error("Try to schedule event in the past!");
        }

        if (is_near_event(event)) {
            m_near_events.add(std::move(event), m_current_event_local_time);
        } else {
            m_far_events.emplace(std::move(event));
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

    [[nodiscard]] inline bool is_near_event(
        const std::unique_ptr<Event>& event) {
        return event->get_time() <
               m_current_event_local_time + M_MAX_COUNTSORT_CAPACITY;
    }

    inline std::unique_ptr<Event>& top_far_event() {
        return const_cast<std::unique_ptr<Event>&>(m_far_events.top());
    }

    inline void correctify_state() {
        while (!m_far_events.empty()) {
            std::unique_ptr<Event>& event = top_far_event();

            if (is_near_event(event)) {
                m_near_events.add(std::move(event), m_current_event_local_time);
                m_far_events.pop();
            } else {
                break;
            }
        }
    }

    NearEventsStorage<M_MAX_COUNTSORT_CAPACITY.value_nanoseconds()>
        m_near_events;

    std::priority_queue<std::unique_ptr<Event>,
                        std::vector<std::unique_ptr<Event>>, EventComparator>
        m_far_events;

    TimeNs m_current_event_local_time;
};

}  // namespace sim
