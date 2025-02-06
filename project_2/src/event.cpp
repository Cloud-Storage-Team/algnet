#include "event.hpp"
#include <string>

Event::Event(std::uint64_t event_time, std::function<void(EventQueue&)> action, std::uint32_t priority)
        : event_time(event_time), action(action), priority(priority) {}

void Event::perform_action(EventQueue& events) {
    if (action) {
        action(events);
    }
}

bool Event::operator<(const Event& other) const {
    return (this->event_time > other.event_time) || 
           (this->event_time == other.event_time && this->priority > other.priority);
}

void EventQueue::push(std::shared_ptr<Event> event) {
    m_events.push(event);
}

std::shared_ptr<Event> EventQueue::get_and_remove() {
    if (m_events.empty()) {
        return nullptr;
    }
    
    auto event = m_events.top();
    m_events.pop();
    return event;
}

std::shared_ptr<Event> EventQueue::top() {
    if (m_events.empty()) {
        return nullptr;
    }
    
    return m_events.top();
}

void EventQueue::pop() {
    if (!m_events.empty()) {
        m_events.pop();
    }
}

bool EventQueue::empty() {
    return m_events.empty();
}
