#include "event.hpp"

#include "scheduler.hpp"

namespace sim {

Stop::Stop() {}

void Stop::operator()() { Scheduler::get_instance().clear(); }

Generate::Generate(std::uint32_t a_time, std::weak_ptr<IFlow> a_flow) 
    : m_time(a_time), m_flow(a_flow) {}

void Generate::operator()() {
    if (auto flow = m_flow.lock()) {
        flow->try_to_generate(time);
    }
}

}  // namespace sim
