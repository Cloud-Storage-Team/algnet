#include "stop_time_action.hpp"

#include "scheduler/event/stop.hpp"
#include "scheduler/scheduler.hpp"

namespace sim {

NewStopTimeAction::NewStopTimeAction(TimeNs a_time) : m_time(a_time) {}

void NewStopTimeAction::schedule() {
    Scheduler::get_instance().add<Stop>(m_time);
}

}  // namespace sim
