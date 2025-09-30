#pragma once
#include <memory>

namespace sim {

class IAction {
public:
    virtual ~IAction() = default;
    virtual void schedule() = 0;
};

}  // namespace sim
