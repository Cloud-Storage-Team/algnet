#include "event.hpp"
#include <string>

Event::Event(std::uint64_t event_time, std::uint32_t priority): event_time(event_time), priority(priority) {}

bool Event::operator<(const Event& other) const {
    return (this->event_time > other.event_time) || 
           (this->event_time == other.event_time && this->priority > other.priority);
}