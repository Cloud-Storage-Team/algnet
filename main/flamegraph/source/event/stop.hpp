#pragma once
#include "event.hpp"

namespace sim {

/**
 * Stop simulation and clear all events remaining in the Scheduler
 */
class Stop : public Event {
public:
    Stop(TimeNs a_time);
    virtual ~Stop() = default;
    void operator()() final;
};

}  // namespace sim