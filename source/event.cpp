#include "event.hpp"

#include "scheduler.hpp"

namespace sim {

Stop::Stop() {}

Stop::Stop(std::uint32_t a_stop_time) {}

void Stop::operator()() { Scheduler::get_instance().clear(); }

}  // namespace sim
