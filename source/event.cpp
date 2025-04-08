#include "event.hpp"

#include "scheduler.hpp"

namespace sim {

Event::Event(Time a_time): m_time(a_time) {};

Stop::Stop(Time a_time): Event(a_time) {}

void Stop::operator()() { Scheduler::get_instance().clear(); }

Generate::Generate(Time a_time, std::weak_ptr<IFlow> a_flow, Size a_packet_size) : Event(a_time), m_flow(a_flow), m_packet_size(a_packet_size) {}

void Generate::operator()() {
    if (m_flow.expired()) {
        return;
    }
    
    m_flow.lock()->try_to_generate(m_packet_size);

    // TODO: make delay not constant (maybe try_to_generate have to return it)
    const Time GENERATE_DELAY = 7;
    std::unique_ptr<Event> new_event = std::make_unique<Generate>(m_time + GENERATE_DELAY, m_flow, m_packet_size);
    Scheduler::get_instance().add(std::move(new_event));
}

Arrive::Arrive(Time a_time, std::weak_ptr<ILink> a_link, Packet a_packet) : Event(a_time), m_link(a_link), m_packet(a_packet) {};

void Arrive::operator()() {
    if (m_link.expired()) {
        return;
    }
    
    m_link.lock()->process_arrival(m_packet); 
};

Process::Process(Time a_time, std::weak_ptr<IProcessingDevice> a_device): Event(a_time), m_device(a_device) {};

void Process::operator()() {
    if (m_device.expired()) {
        return;
    }
    Time process_time = m_device.lock()->process();

    std::unique_ptr<Event> next_process_event = std::make_unique<Process>(m_time + process_time, m_device);
    Scheduler::get_instance().add(std::move(next_process_event));
};

SendData::SendData(Time a_time, std::weak_ptr<ISender> a_device): Event(a_time), m_device(a_device) {};

void SendData::operator()() {
    if (m_device.expired()) {
        return;
    }
    Time process_time = m_device.lock()->send_data();

    std::unique_ptr<Event> next_process_event = std::make_unique<SendData>(m_time + process_time, m_device);
    Scheduler::get_instance().add(std::move(next_process_event));
};

}  // namespace sim
