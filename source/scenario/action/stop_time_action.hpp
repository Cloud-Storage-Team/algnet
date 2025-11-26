#pragma once

#include "i_action.hpp"
#include "types.hpp"
#include "scheduler.hpp"
#include "event/stop.hpp"

namespace sim{

class StopTimeAction: public IAction{
public:
    StopTimeAction(TimeNs a_time);

    void schedule() final;

private:
    TimeNs m_time;
};

} // namespace sim