#include "stop.hpp"

#include "scheduler.hpp"

namespace sim {

Stop::Stop(TimeNs a_time) : Event(a_time) {}

void Stop::operator()() { Scheduler::get_instance().clear(); }

}  // namespace sim