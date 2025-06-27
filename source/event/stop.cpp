#include "stop.hpp"

#include "scheduler/scheduler.hpp"

namespace sim {

Stop::Stop(Time a_time) : Event(a_time) {}

void Stop::operator()() { Scheduler::get_instance().clear(); }

}