#pragma once
#include <memory>
#include <vector>

#include "types.hpp"

namespace sim {

class IConnection;

class IScenario {
public:
    virtual ~IScenario() = default;
    // Move all actions to the Scheduler
    virtual void start() = 0;
};

}  // namespace sim
