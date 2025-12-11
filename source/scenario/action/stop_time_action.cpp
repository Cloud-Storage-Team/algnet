#include "stop_time_action.hpp"
#include "scheduler.hpp"
#include "event/stop.hpp"

namespace sim{

StopTimeAction::StopTimeAction(TimeNs a_time): m_time(a_time){}

void StopTimeAction::schedule(){
    Scheduler::get_instance().add<Stop>(m_time);
}

} // namespace sim