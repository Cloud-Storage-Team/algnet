#include "scheduler.hpp"

#include <queue>
#include <iostream>

#include "event.hpp"

namespace sim {

bool Scheduler::tick() {
    if (m_events.empty()) {
        return false;
    }

    std::unique_ptr<Event> event =
        std::move(const_cast<std::unique_ptr<Event>&>(m_events.top()));
    m_events.pop();
    std::cout << "Total: " << ++m_cnt << std::endl;
    event->operator()();
    return true;
}

void Scheduler::add(std::unique_ptr<Event> event) {
    m_events.emplace(std::move(event));
    // std::cout << "Size: " << m_events.size() << std::endl;
}

void Scheduler::clear() {
    m_events = std::priority_queue<std::unique_ptr<Event>,
                                   std::vector<std::unique_ptr<Event>>,
                                   EventComparator>();
}

}  // namespace sim
