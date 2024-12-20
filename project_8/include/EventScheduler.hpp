#pragma once
#include "Event.hpp"
#include <queue>
#include <cstdint>

class EventScheduler
{

    
    public:
        EventScheduler() = default;

        
        void Schedule(const uint64_t time , const std::function<void()>& handler );
        
        Event PopNextEvent();

        uint64_t PeekNextEventTime();

        bool Empty() const ;
        
    private:
        std::priority_queue<Event> events_;
};

