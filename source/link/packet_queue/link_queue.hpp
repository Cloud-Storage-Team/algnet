#pragma once

#include "simple_packet_queue.hpp"

namespace sim {

enum class LinkQueueType { FromEgress, ToIngress };

std::string to_string(LinkQueueType type);

// Class for two types of links:
// eggress queue of sourse link device or
// ingress queue of desination link device
class LinkQueue : public IPacketQueue {
public:
    LinkQueue(SizeByte a_max_size, Id a_link_id, LinkQueueType a_type);
    ~LinkQueue() = default;

    bool push(Packet packet) final;
    Packet front() const final;
    void pop() final;

    SizeByte get_size() const final;
    bool empty() const final;
    SizeByte get_max_size() const final;

private:
    SimplePacketQueue m_queue;
    Id m_link_id;
    LinkQueueType m_type;
};
}  // namespace sim