#pragma once
#include "event/event.hpp"

namespace sim {

struct EventComparator {
    bool operator()(const std::unique_ptr<Event>& lhs,
                    const std::unique_ptr<Event>& rhs) const;
};

}