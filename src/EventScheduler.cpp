#include "EventScheduler.hpp"

void EventScheduler::Schedule(double time, const std::function<void()>& handler) {
    events.emplace(time, handler);
}

Event EventScheduler::PopNextEvent() {
    Event result = events.top();
    events.pop();
    return result;
}

double EventScheduler::PeekNextEventTime() const {
    if (events.empty()) {
        return -1.0;
    }
    return events.top().execution_time;
}

bool EventScheduler::Empty() const {
    return events.empty();
}