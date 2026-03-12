#include "call_at_time.hpp"

namespace sim {

CallAtTime::CallAtTime(TimeNs when, std::function<void()> to_call)
    : Event(when), m_to_call(to_call) {}

void CallAtTime::operator()() { m_to_call(); }

}  // namespace sim
