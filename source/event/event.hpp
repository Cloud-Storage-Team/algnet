#pragma once

#include "types.hpp"

namespace sim {

// Base class for event
class Event {
public:
    Event(Time a_time);
    virtual ~Event() = default;
    virtual void operator()() = 0;

    Time get_time() const;
    bool operator>(const Event &other) const;

protected:
    const Time m_time;
};

}  // namespace sim
