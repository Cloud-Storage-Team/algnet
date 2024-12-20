#include "Event.hpp"

Event::Event(std::uint64_t time, const std::function<void()> &handler):
        execution_time(time),
        handler(handler) { }

std::uint64_t Event::GetTime() const {
    return execution_time;
}

void Event::Execute() const {
    handler();
}

bool Event::operator<(const Event& other) const {
    return execution_time > other.execution_time;
}