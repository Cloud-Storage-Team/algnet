#pragma once

#include "types.hpp"

namespace sim {

// Base class for event
class Event {
public:
    Event(TimeNs a_time);
    virtual ~Event() = default;
    virtual void operator()() = 0;

    TimeNs get_time() const;
    bool operator>(const Event &other) const;

protected:
    const TimeNs m_time;
};

}  // namespace sim
