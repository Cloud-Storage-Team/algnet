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
            static void StopAt(uint64_t time);
            static void Schedule(uint64_t delay , std::function<void()> handler);
            static uint64_t Now();
            static inline bool EnableACK = true;

        private:
            static inline uint64_t current_time_;
            static inline EventScheduler scheduler_;
            static inline uint64_t stop_time_;
            
        
    };
}