#include "event.hpp"
#include "express_pass/ep_event.hpp"

#include "scheduler.hpp"

namespace sim {

SendCredit::SendCredit(Time a_time, std::weak_ptr<EPFlow> a_flow, Size a_packet_size) : Event(a_time), m_flow(a_flow), m_packet_size(a_packet_size) {}

void SendCredit::operator()() {
    if (m_flow.expired()) {
        return;
    }

    Time sending_delay = m_flow.lock()->send_credit();
    if (sending_delay == 0) {
        return;
    }

    std::unique_ptr<Event> new_event = std::make_unique<SendCredit>(
        m_time + sending_delay, m_flow, m_packet_size);
    Scheduler::get_instance().add(std::move(new_event));
}

RunFeedbackControlLoop::RunFeedbackControlLoop(Time a_time, std::weak_ptr<EPFlow> a_flow) : Event(a_time), m_flow(a_flow) {}

void RunFeedbackControlLoop::operator()() {
    if (m_flow.expired()) {
        return;
    }

    Time rtt = m_flow.lock()->feedback_control_loop();
    if (rtt == 0) {
        return;
    }

    std::unique_ptr<Event> new_event = std::make_unique<RunFeedbackControlLoop>(
        m_time + rtt, m_flow);
    Scheduler::get_instance().add(std::move(new_event));
}

}  // namespace sim