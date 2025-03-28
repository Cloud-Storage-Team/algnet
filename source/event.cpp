#include "event.hpp"

#include "scheduler.hpp"

namespace sim {

Event::Event(std::uint32_t a_time): m_time(a_time) {};

Stop::Stop(std::uint32_t a_time): Event(a_time) {}

void Stop::operator()() { Scheduler::get_instance().clear(); }

Process::Process(std::uint32_t a_time, IProcessingDevice *a_device): Event(a_time), m_device(a_device) {};

void Process::operator()() {
    std::uint32_t process_time = m_device->process();

    std::unique_ptr<Event> next_process_event = std::make_unique<Process>(m_time + process_time, m_device);
    Scheduler::get_instance().add(std::move(next_process_event));
};

SendData::SendData(std::uint32_t a_time, ISender *a_device): Event(a_time), m_device(a_device) {};

void SendData::operator()() {
    std::uint32_t process_time = m_device->send_data();

    std::unique_ptr<Event> next_process_event = std::make_unique<SendData>(m_time + process_time, m_device);
    Scheduler::get_instance().add(std::move(next_process_event));
};


}  // namespace sim
