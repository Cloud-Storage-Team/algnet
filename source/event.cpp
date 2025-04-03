#include "event.hpp"

#include "scheduler.hpp"

namespace sim {

Event::Event(std::uint32_t a_time): m_time(a_time) {};

Stop::Stop(std::uint32_t a_time): Event(a_time) {}

void Stop::operator()() { Scheduler::get_instance().clear(); }

Arrive::Arrive(std::uint32_t a_time, ILink *a_link, Packet a_packet) : link(a_link), packet(a_packet), Event(a_time) {};

void Arrive::operator()() {
    link->process_arrival(packet); 
};

Process::Process(std::uint32_t a_time, std::weak_ptr<IProcessingDevice> a_device): Event(a_time), m_device(a_device) {};

void Process::operator()() {
    if (m_device.expired()) {
        return;
    }
    std::uint32_t process_time = m_device.lock()->process();

    std::unique_ptr<Event> next_process_event = std::make_unique<Process>(m_time + process_time, m_device);
    Scheduler::get_instance().add(std::move(next_process_event));
};

SendData::SendData(std::uint32_t a_time, std::weak_ptr<ISender> a_device): Event(a_time), m_device(a_device) {};

void SendData::operator()() {
    if (m_device.expired()) {
        return;
    }
    std::uint32_t process_time = m_device.lock()->send_data();

    std::unique_ptr<Event> next_process_event = std::make_unique<SendData>(m_time + process_time, m_device);
    Scheduler::get_instance().add(std::move(next_process_event));
};


}  // namespace sim
