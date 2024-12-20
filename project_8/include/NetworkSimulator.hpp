#pragma once

#include "EventScheduler.hpp"
#include <cstdint>
#include <vector>

namespace NetworkSimulator
{
    class NetworkSimulator 
    {
        public:
            NetworkSimulator() = default;


            void Run() const ;
            void StopAt(uint64_t time);
            void Schedule(uint64_t delay , std::function<void()> handler);
            uint64_t Now() const ;



        private:
            uint64_t current_time_;
            EventScheduler scheduler_;
            uint64_t stop_time_;
        
    };
}