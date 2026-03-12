#pragma once
#include <memory>

namespace sim {

class INewAction {
public:
    virtual ~INewAction() = default;
    virtual void schedule() = 0;
};

}  // namespace sim
