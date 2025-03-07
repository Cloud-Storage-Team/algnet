#include "event.hpp"

#include "scheduler.hpp"

namespace sim {

Stop::Stop() {}

void Stop::operator()() { Scheduler::get_instance().clear(); }

}  // namespace sim
