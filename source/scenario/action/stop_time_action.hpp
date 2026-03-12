#pragma once

#include "i_new_action.hpp"
#include "types.hpp"

namespace sim {

class NewStopTimeAction : public INewAction {
public:
    NewStopTimeAction(TimeNs a_time);

    void schedule() final;

private:
    TimeNs m_time;
};

}  // namespace sim