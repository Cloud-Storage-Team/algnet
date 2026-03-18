#pragma once

#include <deque>
#include <memory>
#include <queue>

#include "event/event.hpp"
#include "types.hpp"

namespace sim {

struct EventComparator {
    bool operator()(const std::unique_ptr<Event>& lhs,
                    const std::unique_ptr<Event>& rhs) const;
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
            add_near_event(std::move(event));
        } else {
            m_far_events.emplace(std::move(event));
        }
    }

    void clear();  // Clear all events
    bool tick();
    TimeNs get_current_time();

private:
    static constexpr inline TimeNs M_MAX_COUNTSORT_CAPACITY = TimeNs(100'000);

    // Private constructor to prevent instantiation
    Scheduler() : m_current_event_local_time(TimeNs(0)) {}
    // No copy constructor and assignment operators
    Scheduler(const Scheduler&) = delete;
    Scheduler& operator=(const Scheduler&) = delete;

    [[nodiscard]] inline bool is_near_event(
        const std::unique_ptr<Event>& event) {
        return event->get_time() <
               m_current_event_local_time + M_MAX_COUNTSORT_CAPACITY;
    }

    inline void add_near_event(std::unique_ptr<Event> event) {
        std::uint32_t relative_event_time =
            (event->get_time() - m_current_event_local_time)
                .value_nanoseconds();

        if (m_near_events.size() <= relative_event_time) {
            m_near_events.resize(relative_event_time + 1);
        }

        m_near_events[relative_event_time].emplace(std::move(event));
    }

    inline std::unique_ptr<Event> take_top_far_event() {
        std::unique_ptr<Event> event =
            std::move(const_cast<std::unique_ptr<Event>&>(m_far_events.top()));
        m_far_events.pop();
        return event;
    }

    inline void correctify_state() {
        while (!m_far_events.empty()) {
            const std::unique_ptr<Event>& event = m_far_events.top();

            if (is_near_event(event)) {
                add_near_event(take_top_far_event());
            } else {
                break;
            }
        }
    }

    std::deque<std::queue<std::unique_ptr<Event>>> m_near_events;

    std::priority_queue<std::unique_ptr<Event>,
                        std::vector<std::unique_ptr<Event>>, EventComparator>
        m_far_events;

    TimeNs m_current_event_local_time;
};

}  // namespace sim
