#include "Event.hpp"

Event::Event(std::uint64_t time, const std::function<void()> &handler):
    execution_time(time),
    handler(handler) { }

void Event::Execute() const {
    handler();
}

bool Event::operator<(const Event& other) const {
    return execution_time > other.execution_time;
}