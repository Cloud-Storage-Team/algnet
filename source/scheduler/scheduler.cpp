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

    if (!m_near_events.empty()) {
        run_event(m_near_events.pop_first());
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
    m_near_events =
        NearEventsStorage(M_MAX_COUNTSORT_CAPACITY.value_nanoseconds());
    m_current_event_local_time = TimeNs(0);
}

TimeNs Scheduler::get_current_time() { return m_current_event_local_time; };

}  // namespace sim
