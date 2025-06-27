#pragma once
#include "event.hpp"
#include "flow/interfaces/i_flow.hpp"

namespace sim {

/**
 * Run new flow at specified time
 */
class StartFlow : public Event {
public:
    StartFlow(Time a_time, std::weak_ptr<IFlow> a_flow);
    ~StartFlow() = default;
    void operator()() final;

private:
    std::weak_ptr<IFlow> m_flow;
};

}