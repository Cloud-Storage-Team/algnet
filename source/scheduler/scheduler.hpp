#pragma once

#include <memory>
#include <queue>

#include "types.hpp"

#define SIMPLE_SCHEDULER 1
#define ASYNC_SCHEDULER 2

#define DEFAULT_SCHEDULER SIMPLE_SCHEDULER

#if !defined(SCHEDULER)
#define SCHEDULER DEFAULT_SCHEDULER
#endif

#if SCHEDULER == SIMPLE_SCHEDULER
#include "simple_scheduler.hpp"
using SchedulerBase = sim::SimpleScheduler;
#elif SCHEDULER == ASYNC_SCHEDULER
#include "async_scheduler.hpp"
using SchedulerBase = sim::AsyncScheduler;
#else
#error "unknown SCHEDULER value"
#endif

namespace sim {

// Scheduler is implemented as a Singleton class
// which provides a global access to a single instance
class Scheduler : public SchedulerBase {
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
