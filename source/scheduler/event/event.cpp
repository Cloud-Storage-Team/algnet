#include "event.hpp"

namespace sim {
bool Event::operator>(const Event &other) const {
    return m_time > other.m_time;
}
}  // namespace sim