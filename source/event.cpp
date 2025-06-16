#include "event.hpp"

#include "metrics/metrics_collector.hpp"
#include "scheduler.hpp"

namespace sim {

Event::Event(Time a_time) : m_time(a_time) {};

Time Event::get_time() const { return m_time; }

Generate::Generate(Time a_time, std::weak_ptr<IFlow> a_flow, Size a_packet_size)
    : Event(a_time), m_flow(a_flow), m_packet_size(a_packet_size) {}

void Generate::operator()() {
    if (m_flow.expired()) {
        return;
    }

    Time generate_delay = m_flow.lock()->create_new_data_packet();
    if (generate_delay == 0) {
        return;
    }

    auto new_event = std::make_unique<Generate>(m_time + generate_delay, m_flow,
                                                m_packet_size);
    Scheduler::get_instance().add(std::move(new_event));
}

Arrive::Arrive(Time a_time, std::weak_ptr<ILink> a_link, Packet a_packet)
    : Event(a_time), m_link(a_link), m_packet(a_packet) {};

void Arrive::operator()() {
    if (m_link.expired()) {
        return;
    }

    m_link.lock()->process_arrival(m_packet);
};

Process::Process(Time a_time, std::weak_ptr<IProcessingDevice> a_device)
    : Event(a_time), m_device(a_device) {};

void Process::operator()() {
    if (m_device.expired()) {
        return;
    }

    Time process_time = m_device.lock()->process();

    // TODO: think about better way of cancelling event rescheduling and
    // signaling errors
    if (process_time == 0) {
        return;
    }

    std::unique_ptr<Event> next_process_event =
        std::make_unique<Process>(m_time + process_time, m_device);
    Scheduler::get_instance().add(std::move(next_process_event));
};

SendData::SendData(Time a_time, std::weak_ptr<IHost> a_device)
    : Event(a_time), m_device(a_device) {};

void SendData::operator()() {
    if (m_device.expired()) {
        return;
    }

    Time process_time = m_device.lock()->send_data();

    // TODO: think about better way of cancelling event rescheduling
    if (process_time == 0) {
        return;
    }

    std::unique_ptr<Event> next_process_event =
        std::make_unique<SendData>(m_time + process_time, m_device);
    Scheduler::get_instance().add(std::move(next_process_event));
};

Stop::Stop(Time a_time) : Event(a_time) {}

void Stop::operator()() { Scheduler::get_instance().clear(); }

StartFlow::StartFlow(Time a_time, std::weak_ptr<IFlow> a_flow)
    : Event(a_time), m_flow(a_flow) {}

void StartFlow::operator()() {
    if (m_flow.expired()) {
        return;
    }

    m_flow.lock()->start();
}

TcpMetric::TcpMetric(Time a_time, std::weak_ptr<ITcpFlow> a_flow)
    : Event(a_time), m_flow(a_flow) {};

void TcpMetric::operator()() {
    if (m_flow.expired()) {
        return;
    }

    auto flow = m_flow.lock();

    double cwnd = flow->get_cwnd();
    MetricsCollector::get_instance().add_cwnd(flow->get_id(), m_time, cwnd);

    auto next_metrics_event =
        std::make_unique<TcpMetric>(m_time + DELAY, m_flow);
    Scheduler::get_instance().add(std::move(next_metrics_event));
}

}  // namespace sim
