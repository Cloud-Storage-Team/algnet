#include "event.hpp"

#include "scheduler.hpp"

namespace sim {

Generate::Generate(Time a_time, std::weak_ptr<IFlow> a_flow, Size a_packet_size) : m_flow(a_flow), m_packet_size(a_packet_size), m_time(a_time) {}

void Generate::operator()() {
    if (m_flow.expired()) {
        return;
    }

    Time generate_delay = m_flow.lock()->create_new_data_packet();
    if (generate_delay == 0) {
        return;
    }

    BaseEvent new_event =
        Generate(m_time + generate_delay, m_flow, m_packet_size);
    Scheduler::get_instance().add(std::move(new_event));
}

Time Generate::get_time() const { return m_time; }

Arrive::Arrive(Time a_time, std::weak_ptr<ILink> a_link, Packet a_packet) : m_link(a_link), m_packet(a_packet), m_time(a_time) {};

void Arrive::operator()() {
    if (m_link.expired()) {
        return;
    }

    m_link.lock()->process_arrival(m_packet);
};

Time Arrive::get_time() const { return m_time; }

Process::Process(Time a_time, std::weak_ptr<IProcessingDevice> a_device): m_device(a_device), m_time(a_time) {};

void Process::operator()() {
    if (m_device.expired()) {
        return;
    }

    Time process_time = m_device.lock()->process();

    BaseEvent next_process_event = Process(m_time + process_time, m_device);
    Scheduler::get_instance().add(std::move(next_process_event));
};

Time Process::get_time() const { return m_time; }

SendData::SendData(Time a_time, std::weak_ptr<ISender> a_device): m_device(a_device), m_time(a_time) {};

void SendData::operator()() {
    if (m_device.expired()) {
        return;
    }

    Time process_time = m_device.lock()->send_data();

    BaseEvent next_process_event = SendData(m_time + process_time, m_device);
    Scheduler::get_instance().add(std::move(next_process_event));
};

Time SendData::get_time() const { return m_time; }

Stop::Stop(Time a_time): m_time(a_time) {}

void Stop::operator()() { Scheduler::get_instance().clear(); }

Time Stop::get_time() const { return m_time; }

StartFlow::StartFlow(Time a_time, std::weak_ptr<IFlow> a_flow) : m_flow(a_flow), m_time(a_time) {}

Time StartFlow::get_time() const { return m_time; }

void StartFlow::operator()() {
    if (m_flow.expired()) {
        return;
    }

    m_flow.lock()->start();
}

BaseEvent::BaseEvent(const Generate& e) : event(e) {}
BaseEvent::BaseEvent(const Arrive& e) : event(e) {}
BaseEvent::BaseEvent(const Process& e) : event(e) {}
BaseEvent::BaseEvent(const SendData& e) : event(e) {}
BaseEvent::BaseEvent(const Stop& e) : event(e) {}
BaseEvent::BaseEvent(const StartFlow& e) : event(e) {}

void BaseEvent::operator()() {
    std::visit([&](auto real_event) { real_event(); }, event);
}
bool BaseEvent::operator>(const BaseEvent& other) const {
    return get_time() > other.get_time();
}
Time BaseEvent::get_time() const {
    return std::visit([&](auto real_event) { return real_event.get_time(); },
                      event);
}

}  // namespace sim
