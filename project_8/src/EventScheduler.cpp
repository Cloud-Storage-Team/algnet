#include "EventScheduler.hpp"

void EventScheduler::Schedule(const uint64_t time , const std::function<void()>& handler )
{
    Event event =Event(time ,handler);
    events_.push(event);
}
        
Event EventScheduler::PopNextEvent()
{
    if(!events_.empty())
    {
        Event event=events_.top();
        events_.pop();
        return event;
    }
    else 
    {
        return Event();
    }
}

uint64_t EventScheduler::PeekNextEventTime()
{
    if(!events_.empty())
    {
        Event event=events_.top();
        events_.pop();
        return event.GetTime();
    }
    else 
    {
        return Event().GetTime();
    }
}


bool EventScheduler::Empty() const
{
    return events_.empty();
}