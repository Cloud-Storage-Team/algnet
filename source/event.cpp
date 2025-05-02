#include "event.hpp"

#include "scheduler.hpp"
#include <string>

namespace sim {

Event::Event(Time a_time): m_time(a_time) {};

Time Event::get_time() const {
    return m_time;
}

Stop::Stop(Time a_time): Event(a_time) {}

void Stop::operator()() { Scheduler::get_instance().clear(); }

Generate::Generate(Time a_time, std::weak_ptr<IFlow> a_flow, Size a_packet_size) : Event(a_time), m_flow(a_flow), m_packet_size(a_packet_size) {}

void Generate::operator()() {
    if (m_flow.expired()) {
        return;
    }

    // LOG_WARN("Created data packet at: " + std::to_string(m_time));
    Time generate_delay = m_flow.lock()->create_new_data_packet();
    if (generate_delay == 0) {
        return;
    }

    std::unique_ptr<Event> new_event = std::make_unique<Generate>(
        m_time + generate_delay, m_flow, m_packet_size);
    Scheduler::get_instance().add(std::move(new_event));
}

Arrive::Arrive(Time a_time, std::weak_ptr<ILink> a_link, Packet a_packet) : Event(a_time), m_link(a_link), m_packet(a_packet) {};

void Arrive::operator()() {
    if (m_link.expired()) {
        return;
    }
    
    // LOG_WARN("Packet arrived at: " + std::to_string(m_time));
    m_link.lock()->process_arrival(m_packet); 
};

Process::Process(Time a_time, std::weak_ptr<IProcessingDevice> a_device): Event(a_time), m_device(a_device) {};

void Process::operator()() {
    if (m_device.expired()) {
        return;
    }
    // LOG_WARN("Packet processed at: " + std::to_string(m_time));
    Time process_time = m_device.lock()->process(m_time);

    std::unique_ptr<Event> next_process_event = std::make_unique<Process>(m_time + process_time, m_device);
    Scheduler::get_instance().add(std::move(next_process_event));
};

SendData::SendData(Time a_time, std::weak_ptr<ISender> a_device): Event(a_time), m_device(a_device) {};

void SendData::operator()() {
    if (m_device.expired()) {
        return;
    }
    // LOG_WARN("Packet sent at: " + std::to_string(m_time));
    Time process_time = m_device.lock()->send_data(m_time);

    std::unique_ptr<Event> next_process_event = std::make_unique<SendData>(m_time + process_time, m_device);
    Scheduler::get_instance().add(std::move(next_process_event));
};

}  // namespace sim
