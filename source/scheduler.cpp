#include "scheduler.hpp"

#include <queue>

#include "event.hpp"

namespace sim {

bool Scheduler::tick() {
    if (m_events.empty()) {
        return false;
    }

    EventVariant event = std::move(m_events.top());
    m_events.pop();
    m_current_event_local_time =  std::visit([](const auto& event){ return event.get_time(); }, event); 
    std::visit([](auto& e) { e.operator()(); }, event);
    return true;
}

void Scheduler::add(EventVariant event) { m_events.emplace(std::move(event)); }

void Scheduler::clear() {
    m_events = std::priority_queue<EventVariant, std::vector<EventVariant>,
                                   EventComparator>();
}

Time Scheduler::get_current_time() {
    return m_current_event_local_time;
};

Time Scheduler::increase_current_time(Time offset) {
    m_current_event_local_time += offset;
    return m_current_event_local_time;
};

}  // namespace sim
