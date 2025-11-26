#include "stop_time_action.hpp"


namespace sim{

StopTimeAction::StopTimeAction(TimeNs a_time): m_time(a_time){}

void StopTimeAction::schedule(){
    Scheduler::get_instance().add<Stop>(this->m_time);
    
}

} // namespace sim