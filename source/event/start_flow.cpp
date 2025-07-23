#include "start_flow.hpp"

namespace sim {

StartFlow::StartFlow(TimeNs a_time, std::weak_ptr<IFlow> a_flow)
    : Event(a_time), m_flow(a_flow) {}

void StartFlow::operator()() {
    if (m_flow.expired()) {
        return;
    }

    m_flow.lock()->start();
}

}