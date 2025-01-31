#include "EventScheduler.hpp"

void EventScheduler::Schedule(std::uint64_t time, const std::function<void()>& handler) {
    events.emplace(time, handler);
}

Event EventScheduler::PopNextEvent() {
    Event result = events.top();
    events.pop();
    return result;
}

std::uint64_t EventScheduler::PeekNextEventTime() const {
    if (events.empty()) {
        return 0;
    }
    return events.top().execution_time;
}

bool EventScheduler::Empty() const {
    return events.empty();
}
