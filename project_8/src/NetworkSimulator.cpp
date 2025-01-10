#include "NetworkSimulator.hpp"
#include "EventScheduler.hpp"
#include <memory>
void NetworkSimulator::NetworkSimulator::Run() const 
{
    
    while(!scheduler_.Empty() || current_time_<stop_time_)
    {
        
    }
}

void NetworkSimulator::NetworkSimulator::StopAt(uint64_t time) 
{
    stop_time_=time;
}

void NetworkSimulator::NetworkSimulator::Schedule(uint64_t delay , std::function<void()> handler) 
{
    scheduler_.Schedule(current_time_+delay,handler);
}

uint64_t NetworkSimulator::NetworkSimulator::Now()
{
    return current_time_;
}