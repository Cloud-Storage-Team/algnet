#include "scheduler.hpp"

#include <iostream>
#include <queue>

#include "event.hpp"

namespace sim {

void Scheduler::tick() {
    if (!m_events.empty()) {
        Event* event = m_events.top();
        m_events.pop();
        (*event)();
        delete event;
    } else {
        std::cout << "No events to process." << std::endl;
    }
}

void Scheduler::add(Event& event) { m_events.emplace(event); }

}  // namespace sim
