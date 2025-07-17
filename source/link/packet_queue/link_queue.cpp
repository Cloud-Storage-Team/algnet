#include "link_queue.hpp"

#include "metrics/metrics_collector.hpp"
#include "scheduler.hpp"
#include "simple_packet_queue.hpp"

namespace sim {

std::string to_string(LinkQueueType type) {
    switch (type) {
        case LinkQueueType::FromEgress:
            return "from_ingress_queue_size";
        case LinkQueueType::ToIngress:
            return "to_ingress_queue_size";
        default:
            LOG_ERROR(fmt::format("Undefined link queue type: {}",
                                  static_cast<int>(type)));
            return "queue_size";
    }
}

LinkQueue::LinkQueue(SimplePacketQueue a_queue, Id a_link_id,
                     LinkQueueType a_type)
    : m_queue(std::move(a_queue)), m_link_id(a_link_id), m_type(a_type) {}

bool LinkQueue::push(Packet packet) {
    bool result = m_queue.push(std::move(packet));
    MetricsCollector::get_instance().add_queue_size(
        m_link_id, Scheduler::get_instance().get_current_time(),
        m_queue.get_size(), m_type);
    return result;
}

Packet LinkQueue::front() { return m_queue.front(); }

void LinkQueue::pop() {
    m_queue.pop();
    MetricsCollector::get_instance().add_queue_size(
        m_link_id, Scheduler::get_instance().get_current_time(),
        m_queue.get_size(), m_type);
}

Size LinkQueue::get_size() const { return m_queue.get_size(); }

bool LinkQueue::empty() const { return m_queue.empty(); }

Size LinkQueue::get_max_size() const { return m_queue.get_max_size(); }

}  // namespace sim