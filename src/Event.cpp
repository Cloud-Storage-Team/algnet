#include "Event.hpp"

Event::Event(std::uint64_t time, const std::function<void()> &handler):
        m_execution_time(time),
        m_handler(handler) { }

void Event::Execute() const {
    m_handler();
}

bool Event::operator<(const Event& other) const {
    return m_execution_time > other.m_execution_time;
}