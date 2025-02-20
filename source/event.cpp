#include "event.hpp"

namespace sim
{
Event::Event(std::uint32_t a_time) {
    time = a_time;
}

Event::~Event() {}

Stop::Stop(std::uint32_t a_time): Event(a_time) {}

Stop::~Stop() {}

void Stop::operator()(){}
} // namespace sim
