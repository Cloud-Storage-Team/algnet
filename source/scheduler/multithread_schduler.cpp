#include <queue>

#include "event.hpp"
#include "scheduler/multithread_scheduler.hpp"

namespace sim {

bool MultithreadScheduler::tick() {
    std::unique_ptr<Event> event;
    {
        std::lock_guard<SpinLock> lock(m_lock);
        if (m_events.empty()) {
            return false;
        }

        event = std::move(const_cast<std::unique_ptr<Event>&>(m_events.top()));
    }

    // to first coroutive
    m_events.pop();

    // to second coroutine
    m_current_event_local_time = event->get_time();
    event->operator()();
    
    return true;
}

void MultithreadScheduler::clear() {
    m_events = std::priority_queue<std::unique_ptr<Event>,
                                   std::vector<std::unique_ptr<Event>>,
                                   EventComparator>();
}

Time MultithreadScheduler::get_current_time() {
    return m_current_event_local_time;
};

}  // namespace sim
