// #include "scheduler.hpp"

// void Scheduler::push(std::shared_ptr<Event> event) {
//     m_events.push(event);
// }

// std::shared_ptr<Event> Scheduler::get_and_remove() {
//     if (m_events.empty()) {
//         return nullptr;
//     }
    
//     auto event = m_events.top();
//     m_events.pop();
//     return event;
// }

// std::shared_ptr<Event> Scheduler::top() {
//     if (m_events.empty()) {
//         return nullptr;
//     }
    
//     return m_events.top();
// }

// void Scheduler::pop() {
//     if (!m_events.empty()) {
//         m_events.pop();
//     }
// }

// bool Scheduler::empty() {
//     return m_events.empty();
// }
