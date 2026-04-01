#include "stop_time_action.hpp"

#include "scheduler/scheduler.hpp"

namespace sim {

StopTimeAction::StopTimeAction(TimeNs a_time) : m_time(a_time) {}

void StopTimeAction::schedule() {
    Scheduler::get_instance().add(m_time,
                                  []() { Scheduler::get_instance().clear(); });
}

}  // namespace sim
