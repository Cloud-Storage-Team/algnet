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
    // event->operator()();
    std::visit([](auto& e) { e.operator()(); }, event);
    return true;
}

void Scheduler::add(EventVariant event) { m_events.emplace(std::move(event)); }

void Scheduler::clear() {
    m_events = std::priority_queue<EventVariant, std::vector<EventVariant>,
                                   EventComparator>();
}

}  // namespace sim
