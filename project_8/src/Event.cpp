#pragma once
#include "Event.hpp"

Event::Event(uint64_t time, const std::function<void()>& handler) : 
    execution_time_(time) ,
    handler_(handler) { }

bool Event::operator<(const Event& other) const 
{
    return this->execution_time_<other.execution_time_;
}

uint64_t Event::GetTime()
{
    return execution_time_;
}

void Event::Execute() const 
{
    handler_();
}