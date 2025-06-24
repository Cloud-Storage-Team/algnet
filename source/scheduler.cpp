#include "scheduler.hpp"

#include <queue>

#include "event.hpp"

namespace sim {

bool EventComparator::operator()(const std::unique_ptr<Event>& lhs,
                                 const std::unique_ptr<Event>& rhs) const {
    return (*lhs.get()) > (*rhs.get());
}

bool Scheduler::tick() {
    if (m_events.empty()) [[unlikely]] {
        return false;
    }

    std::unique_ptr<Event> event =
        std::move(const_cast<std::unique_ptr<Event>&>(m_events.top()));
    m_events.pop();
    m_current_event_local_time = event->get_time();
    event->operator()();
    return true;
}

void Scheduler::clear() {
    m_events = std::priority_queue<std::unique_ptr<Event>,
                                   std::vector<std::unique_ptr<Event>>,
                                   EventComparator>();
}

Time Scheduler::get_current_time() { return m_current_event_local_time; };

}  // namespace sim
