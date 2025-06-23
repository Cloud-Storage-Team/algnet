#include <cppcoro/sync_wait.hpp>
#include <cppcoro/when_all.hpp>
#include <queue>

#include "event.hpp"
#include "scheduler/multithread_scheduler.hpp"

namespace sim {

bool MultithreadScheduler::tick() {
    cppcoro::task<> execute_task;
    {
        std::lock_guard lock(m_lock);
        if (m_events.empty()) {
            return false;
        }

        execute_task =
            execute_event(const_cast<std::unique_ptr<Event>&&>(m_events.top()));
    }

    cppcoro::sync_wait(cppcoro::when_all(pop(), std::move(execute_task)));

    return true;
}

void MultithreadScheduler::clear() {
    std::lock_guard lock(m_lock);
    m_events = std::priority_queue<std::unique_ptr<Event>,
                                   std::vector<std::unique_ptr<Event>>,
                                   EventComparator>();
}

Time MultithreadScheduler::get_current_time() {
    return m_current_event_local_time;
};

cppcoro::task<> MultithreadScheduler::pop() {
    std::lock_guard lock(m_lock);
    m_events.pop();
    co_return;
}

cppcoro::task<> MultithreadScheduler::execute_event(
    std::unique_ptr<Event> event) {
    m_current_event_local_time = event->get_time();
    event->operator()();
    co_return;
}

}  // namespace sim
