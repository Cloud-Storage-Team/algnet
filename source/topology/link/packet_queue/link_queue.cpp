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
    if (result) {
        m_ctx.packets_transmitted++;
    } else {
        m_ctx.packets_dropped++;
    }
    record_size();
    return result;
}

const Packet& LinkQueue::front() const { return m_queue.front(); }

Packet& LinkQueue::front() { return m_queue.front(); }

void LinkQueue::pop() {
    m_queue.pop();
    record_size();
}

bool LinkQueue::empty() const { return m_queue.empty(); }

SizeByte LinkQueue::get_max_size() const { return m_queue.get_max_size(); }

std::shared_ptr<const MetricsStorage> LinkQueue::get_queue_size_storage()
    const {
    return m_queue_size_storage;
}

LinkQueueType LinkQueue::get_type() const { return m_type; }

const LinkQueueContext& LinkQueue::get_ctx() const { return m_ctx; }

void LinkQueue::write_queue_metrics_to_csv(std::ofstream& out) const {
    if (m_type == LinkQueueType::FromEgress) {
        out << "Egress buffer of " << m_link_id << "\n";
    } else {
        out << "Ingress buffer of " << m_link_id << "\n";
    }

    out << "Maximal size"
        << ", Average size"
        << ", Peak size"
        << ", Packets transmitted"
        << ", Packets dropped"
        << ", Drop percent\n";

    const utils::Statistics<SizeByte> statistics = m_ctx.size_statistics;

    SizeByte average = statistics.get_mean().value_or(SizeByte(0ul));

    double drop_percent = 0;
    double total_packets =
        static_cast<double>(m_ctx.packets_dropped + m_ctx.packets_transmitted);
    if (total_packets > 0) {
        drop_percent = m_ctx.packets_dropped / total_packets;
    }

    out << get_max_size();
    out << ", " << average;
    out << ", " << statistics.get_peak().value_or(SizeByte(0ul));
    out << ", " << m_ctx.packets_transmitted;
    out << ", " << m_ctx.packets_dropped;
    out << ", " << drop_percent;
    out << "\n\n";
}

void LinkQueue::record_size() {
    TimeNs now = Scheduler::get_instance().get_current_time();
    SizeByte queue_size = m_queue.get_size();
    m_queue_size_storage->add_record(now, queue_size.value());
    m_ctx.size_statistics.add_record(queue_size);
    m_ctx.size = queue_size;
}

}  // namespace sim
