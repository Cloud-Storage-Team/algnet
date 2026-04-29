#include "scheduler/scheduler.hpp"

#include <cassert>
#include <queue>

#include "event.hpp"

namespace sim {

bool Scheduler::tick() {
    if (!m_near_events.empty()) {
        m_near_events.tick([this](TimeNs new_cur_time) {
            m_current_event_local_time = new_cur_time;
        });
        correctify_state();
        return true;
    }
    // no near events

    if (m_far_events.empty()) {
        return false;
    }

    // no near events, but have some far ones => run first from far ones
    NewEvent far_event = std::move(top_far_event());
    m_far_events.pop();

    // fetch time
    m_near_events.set_current_time(far_event.time);
    m_current_event_local_time = far_event.time;

    // invoke event & correctify state
    std::invoke(far_event.call);
    correctify_state();
    return true;
}

uint32_t Scheduler::tick_to(TimeNs time_point) {
    time_point += TimeNs(1);
    if (time_point <= m_current_event_local_time) {
        return 0;
    }
    bool arrived = false;
    uint32_t events_count = 0;
    add(time_point, [&arrived]() { arrived = true; });
    while (!arrived && tick()) {
        events_count++;
    }
    if (arrived) {
        events_count--;
    }
    return events_count;
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
