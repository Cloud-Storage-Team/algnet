#pragma once

#include "types.hpp"

namespace sim {

// Base class for event
class Event {
public:
    explicit inline Event(TimeNs a_time) : m_time(a_time) {}
    virtual ~Event() = default;
    virtual void operator()() = 0;

    inline TimeNs get_time() const { return m_time; }
    bool operator>(const Event &other) const;

protected:
    const TimeNs m_time;
};

}  // namespace sim
