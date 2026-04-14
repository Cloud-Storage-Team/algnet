#pragma once

#include "metrics/metrics_storage.hpp"
#include "simple_packet_queue.hpp"
#include "utils/statistics.hpp"

namespace sim {

enum class LinkQueueType { FromEgress, ToIngress };

struct LinkQueueContext {
    SizeByte size;
    utils::Statistics<SizeByte> size_statistics;
    uint64_t packets_transmitted;
    uint64_t packets_dropped;
};

std::string to_string(LinkQueueType type);

// Class for two types of links:
// eggress queue of sourse link device or
// ingress queue of desination link device
class LinkQueue {
public:
    LinkQueue(SizeByte a_max_size, Id a_link_id, LinkQueueType a_type);
    ~LinkQueue() = default;

    bool push(const Packet& packet);
    const Packet& front() const;
    Packet& front();
    void pop();

    SizeByte get_max_size() const;
    LinkQueueType get_type() const;
    bool empty() const;
    const LinkQueueContext& get_ctx() const;
    void write_queue_metrics_to_csv(std::ofstream& out) const;

    std::shared_ptr<const MetricsStorage> get_queue_size_storage() const;

private:
    void record_size();

    SimplePacketQueue m_queue;
    Id m_link_id;
    LinkQueueType m_type;
    LinkQueueContext m_ctx;

    std::shared_ptr<MetricsStorage> m_queue_size_storage;
};
}  // namespace sim
