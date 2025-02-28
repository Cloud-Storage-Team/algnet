#include "event.hpp"

#include "flow.hpp"
#include "scheduler.hpp"

namespace sim {

Event::Event(std::uint32_t a_time) : m_time(a_time) {}

std::uint32_t Event::get_time() const { return m_time; }

Generate::Generate(std::uint32_t a_time, Flow* a_flow,
                   std::uint32_t a_packet_size)
    : Event(a_time), m_flow(a_flow), m_packet_size(a_packet_size) {}

void Generate::operator()() {
    m_flow->try_to_generate(m_packet_size);

    // TODO: make delay not constant (maybe try_to_generate have to return it)
    const std::uint32_t GENERATE_DELAY = 7;  // ms
    Generate* new_event =
        new Generate(m_time + GENERATE_DELAY, m_flow, m_packet_size);
    Scheduler::get_instance().add(new_event);
}

Arrive::Arrive(std::uint32_t a_time, Link* a_link, Packet a_packet)
    : Event(a_time), m_link(a_link), m_packet(a_packet) {}

void Arrive::operator()() { m_link->schedule_arrival(m_packet); }

Process::Process(std::uint32_t a_time, Device* a_device)
    : Event(a_time), m_device(a_device) {}

void Process::operator()() {
    m_device->process();

    // TODO: make delay not constant (e.g. it may be a return value of
    // m_device->process())
    const std::uint32_t PROCESS_DELAY = 5;  // ms
    Process* next_process_event = new Process(m_time + PROCESS_DELAY, m_device);
    Scheduler::get_instance().add(next_process_event);
}

Stop::Stop(std::uint32_t a_time) : Event(a_time) {}

void Stop::operator()() { Scheduler::get_instance().clear(); }
}  // namespace sim
