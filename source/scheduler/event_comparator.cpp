#include "event_comparator.hpp"

namespace sim {

bool EventComparator::operator()(const std::unique_ptr<Event>& lhs,
                                 const std::unique_ptr<Event>& rhs) const {
    return (*lhs.get()) > (*rhs.get());
}
}  // namespace sim
