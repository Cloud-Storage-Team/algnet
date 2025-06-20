#include <utility>

#include "event_comparator.hpp"
#include "multithread_sheduler.hpp"

namespace sim {

MultithreadScheduler::MultithreadScheduler()
    : m_earliest_event_index(0), m_current_event_local_time(0) {}

void MultithreadScheduler::clear() {
    m_events.clear();
    m_earliest_event_index = 0;
    m_current_event_local_time = 0;
}

Time MultithreadScheduler::get_current_time() const {
    return m_current_event_local_time;
}

bool MultithreadScheduler::tick() {
    if (m_events.empty()) {
        return false;
    }

    m_events.back().swap(m_events[m_earliest_event_index]);
    auto event = std::move(m_events.back());
    m_events.pop_back();

    m_current_event_local_time = event->get_time();
    event->operator()();

    m_earliest_event_index =
        std::min_element(m_events.begin(), m_events.end(),
                         [](std::unique_ptr<Event>& first_ptr,
                            std::unique_ptr<Event>& second_ptr) {
                             return (*second_ptr.get()) > (*first_ptr.get());
                         }) -
        m_events.begin();
    return true;
}

}  // namespace sim