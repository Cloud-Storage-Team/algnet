#include "link_queue.hpp"

#include "scheduler/scheduler.hpp"
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

LinkQueue::LinkQueue(SizeByte a_queue_size, Id a_link_id, LinkQueueType a_type)
    : m_queue(a_queue_size),
      m_link_id(a_link_id),
      m_type(a_type),
      m_queue_size_storage(std::make_shared<MetricsStorage>()) {}

bool LinkQueue::push(const Packet& packet) {
    bool result = m_queue.push(packet);
    record_size();
    return result;
}

const Packet& LinkQueue::front() const { return m_queue.front(); }

Packet& LinkQueue::front() { return m_queue.front(); }

void LinkQueue::pop() {
    m_queue.pop();
    record_size();
}

SizeByte LinkQueue::get_size() const { return m_queue.get_size(); }

bool LinkQueue::empty() const { return m_queue.empty(); }

SizeByte LinkQueue::get_max_size() const { return m_queue.get_max_size(); }

std::shared_ptr<const MetricsStorage> LinkQueue::get_queue_size_storage()
    const {
    return m_queue_size_storage;
}

void LinkQueue::record_size() {
    TimeNs now = Scheduler::get_instance().get_current_time();
    SizeByte queue_size = m_queue.get_size();
    m_queue_size_storage->add_record(now, queue_size.value());
}

}  // namespace sim
