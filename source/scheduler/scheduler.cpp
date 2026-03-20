#include "scheduler/scheduler.hpp"

#include <queue>

#include "event/event.hpp"

namespace sim {

bool Scheduler::tick() {
    auto run_event = [this](NewEvent& event) {
        m_current_event_local_time = event.time;
        std::invoke(event.call);
        correctify_state();
    };

    if (!m_near_events.empty()) {
        NewEvent event = std::move(m_near_events.top());
        m_near_events.pop();
        run_event(event);
        return true;
    }
    // no near events

    if (m_far_events.empty()) {
        return false;
    }

    // no near events, but have some far ones => run first from far ones
    NewEvent far_event = std::move(top_far_event());
    m_far_events.pop();
    run_event(far_event);
    return true;
}

void Scheduler::clear() {
    m_near_events.clear();
    std::priority_queue<NewEvent, std::vector<NewEvent>,
                        std::greater<NewEvent> >()
        .swap(m_far_events);
    m_current_event_local_time = TimeNs(0);
}

TimeNs Scheduler::get_current_time() { return m_current_event_local_time; };

}  // namespace sim
