#pragma once

#include "i_action.hpp"
#include "types.hpp"

namespace sim {

class NewStopTimeAction : public IAction {
public:
    NewStopTimeAction(TimeNs a_time);

    void schedule() final;

private:
    TimeNs m_time;
};

}  // namespace sim
