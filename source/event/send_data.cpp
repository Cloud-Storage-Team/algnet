#include "send_data.hpp"

#include "scheduler.hpp"

namespace sim {

SendData::SendData(TimeNs a_time, std::weak_ptr<IHost> a_device)
    : Event(a_time), m_device(a_device) {};

void SendData::operator()() {
    if (m_device.expired()) {
        return;
    }

    TimeNs process_time = m_device.lock()->send_packet();

    // TODO: think about better way of cancelling event rescheduling
    if (process_time == 0) {
        return;
    }

    Scheduler::get_instance().add<SendData>(m_time + process_time, m_device);
};

}