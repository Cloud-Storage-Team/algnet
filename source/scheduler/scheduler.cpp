#include "scheduler/scheduler.hpp"

#include <queue>

#include "event/event.hpp"

namespace sim {

bool Scheduler::tick() {
    auto run_event = [this](std::unique_ptr<Event> event) {
        m_current_event_local_time = event->get_time();
        correctify_state();
        event->operator()();
    };

    if (!m_near_events.empty()) {
        run_event(m_near_events.take_first());
        return true;
    }
    // no near events

    if (m_far_events.empty()) {
        return false;
    }

    // no near events, but have some far ones => run first from far ones
    run_event(take_top_far_event());
    return true;

    // no near nor fat events => nothing to do
    return false;
}

void Scheduler::clear() {
    m_near_events.clear();
    std::priority_queue<std::unique_ptr<Event>,
                        std::vector<std::unique_ptr<Event>>, EventComparator>()
        .swap(m_far_events);
    m_current_event_local_time = TimeNs(0);
}

TimeNs Scheduler::get_current_time() { return m_current_event_local_time; };

}  // namespace sim
