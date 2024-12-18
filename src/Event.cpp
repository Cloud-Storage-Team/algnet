#include "Event.hpp"

Event::Event(double time, const std::function<void()> &handler):
    execution_time(time),
    handler(handler) { }

double Event::GetTime() const {
    return execution_time;
}

void Event::Execute() const {
    handler();
}

bool Event::operator<(const Event& other) const {
    return execution_time > other.execution_time;
}