#include "process.hpp"

#include "scheduler.hpp"

namespace sim {

Process::Process(TimeNs a_time, std::weak_ptr<IProcessingDevice> a_device)
    : Event(a_time), m_device(a_device) {};

void Process::operator()() {
    if (m_device.expired()) {
        return;
    }

    TimeNs process_time = m_device.lock()->process();

    // TODO: think about better way of cancelling event rescheduling and
    // signaling errors
    if (process_time == TimeNs(0)) {
        return;
    }

    Scheduler::get_instance().add<Process>(m_time + process_time, m_device);
};

}