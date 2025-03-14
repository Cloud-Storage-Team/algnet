#include "event.hpp"

#include "scheduler.hpp"

namespace sim {

Stop::Stop() {}

void Stop::operator()() { Scheduler::get_instance().clear(); }

Generate::Generate(IFlow* flow) : flow(flow) {}

void Generate::operator()() { flow->try_to_generate(time); }

}  // namespace sim
