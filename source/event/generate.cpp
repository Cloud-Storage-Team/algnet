#include "generate.hpp"

#include "scheduler.hpp"

namespace sim {

Generate::Generate(Time a_time, std::weak_ptr<IFlow> a_flow,
                   SizeByte a_packet_size)
    : Event(a_time), m_flow(a_flow), m_packet_size(a_packet_size) {}

void Generate::operator()() {
    if (m_flow.expired()) {
        return;
    }

    Time generate_delay = m_flow.lock()->create_new_data_packet();
    if (generate_delay == 0) {
        return;
    }

    Scheduler::get_instance().add<Generate>(m_time + generate_delay, m_flow,
                                            m_packet_size);
}

}  // namespace sim