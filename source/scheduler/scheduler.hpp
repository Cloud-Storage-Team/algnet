#pragma once

#include <memory>
#include <queue>

#include "simple_scheduler.hpp"
#include "types.hpp"

namespace sim {

// Scheduler is implemented as a Singleton class
// which provides a global access to a single instance
class Scheduler : public
#if !defined(SCHEDULER)
                  SimpleScheduler
#elif SCHEDULER = ASYNC
                  MultithreadScheduler
#else
                  SimpleScheduler
#endif
{
public:
    // Static method to get the instance
    static Scheduler& get_instance() {
        static Scheduler instance;
        return instance;
    }

private:
    // Private constructor to prevent instantiation
    Scheduler() = default;
    // No copy constructor and assignment operators
    Scheduler(const Scheduler&) = delete;
    Scheduler& operator=(const Scheduler&) = delete;
};  // namespace sim

}  // namespace sim
