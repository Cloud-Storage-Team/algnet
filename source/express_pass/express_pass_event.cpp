// #include "event.hpp"
// #include "express_pass/express_pass_event.hpp"

// #include "scheduler.hpp"

// namespace sim {
    
// SendCredit::SendCredit(Time a_time, std::weak_ptr<ExpressPassFlow> a_flow, Size a_packet_size) : Event(a_time), m_flow(a_flow), m_packet_size(a_packet_size) {}

// void SendCredit::operator()() {
//     if (m_flow.expired()) {
//         return;
//     }

//     Time sending_delay = m_flow.lock()->send_credit(m_time);
//     if (sending_delay == 0) {
//         return;
//     }

//     EventVariant new_event = SendCredit(
//         m_time + sending_delay, m_flow, m_packet_size);
//     Scheduler::get_instance().add(std::move(new_event));
// }

// }  // namespace sim
