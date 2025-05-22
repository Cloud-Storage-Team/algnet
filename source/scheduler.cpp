#include "scheduler.hpp"

#include <queue>

#include "event.hpp"

namespace sim {

bool Scheduler::tick() {
    if (m_events.empty()) {
        return false;
    }

    std::unique_ptr<Event> event =
        std::move(const_cast<std::unique_ptr<Event>&>(m_events.top()));
    m_events.pop();
    m_current_event_local_time = event->get_time();
    event->operator()();
    m_current_event_local_time = event->get_time();
    return true;
}

void Scheduler::add(std::unique_ptr<Event> event) {
    m_events.emplace(std::move(event));
}

void Scheduler::clear() {
    m_events = std::priority_queue<std::unique_ptr<Event>,
                                   std::vector<std::unique_ptr<Event>>,
                                   EventComparator>();
}

Time Scheduler::get_current_time() {
    return m_current_event_local_time;
};

}  // namespace sim
