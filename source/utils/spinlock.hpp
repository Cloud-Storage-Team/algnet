#pragma once
#include <atomic>
#include <mutex>
#include <thread>

namespace sim {

class SpinLock {
public:
    SpinLock() = default;
    void lock();
    void unlock();

private:
    std::atomic_flag locked = ATOMIC_FLAG_INIT;
};

}  // namespace sim