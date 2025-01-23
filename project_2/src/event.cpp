#include "event.hpp"
#include <string>

Event::Event(std::uint64_t event_time, std::function<void(std::priority_queue<std::shared_ptr<Event>, std::vector<std::shared_ptr<Event>>, EventComparator>&)> action, std::uint32_t priority)
        : event_time(event_time), action(action), priority(priority) {}

void Event::perform_action(std::priority_queue<std::shared_ptr<Event>, std::vector<std::shared_ptr<Event>>, EventComparator>& events) {
    if (action) {
        action(events);
    }
}

bool Event::operator<(const Event& other) const {
    return (this->event_time > other.event_time) || 
           (this->event_time == other.event_time && this->priority > other.priority);
}