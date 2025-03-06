#include <event.hpp>
#include <flow.hpp>
#include <scheduler.hpp>

namespace sim {

Flow::Flow(ISender *a_src, IReceiver *a_dest) : m_src(a_src), m_dest(a_dest) {}

void Flow::schedule_packet_generation(std::uint32_t time) {
    Generate generate_event(this);
    generate_event.time = time;
    Scheduler::get_instance().add(generate_event);
}

}  // namespace sim