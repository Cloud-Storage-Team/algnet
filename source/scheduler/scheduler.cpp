#include "scheduler/scheduler.hpp"

#include <queue>

#include "event/event.hpp"

namespace sim {

bool EventComparator::operator()(const std::unique_ptr<Event>& lhs,
                                 const std::unique_ptr<Event>& rhs) const {
    return (*lhs.get()) > (*rhs.get());
}

bool Scheduler::tick() {
    auto run_event = [this](std::unique_ptr<Event> event) {
        m_current_event_local_time = event->get_time();
        correctify_state();
        event->operator()();
    };

    while (!m_near_events.empty() && m_near_events.front().empty()) {
        m_near_events.pop_front();
    }
    // now m_near_events is empty or its first storage is not empty

    if (!m_near_events.empty()) {
        // run first near event
        std::unique_ptr<Event> event = std::move(m_near_events[0].front());
        m_near_events[0].pop();
        run_event(std::move(event));
        return true;
    }

    if (!m_far_events.empty()) {
        // no near events, but have some far ones => run first from far ones
        run_event(take_top_far_event());
        return true;
    }

    // no near nor fat events => nothing to do
    return false;
}

void Scheduler::clear() {
    std::priority_queue<std::unique_ptr<Event>,
                        std::vector<std::unique_ptr<Event>>, EventComparator>()
        .swap(m_far_events);
    std::deque<std::queue<std::unique_ptr<Event>>>().swap(m_near_events);
    m_current_event_local_time = TimeNs(0);
}

TimeNs Scheduler::get_current_time() { return m_current_event_local_time; };

}  // namespace sim
