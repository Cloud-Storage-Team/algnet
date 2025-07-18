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
    LinkQueue(SimplePacketQueue a_queue, Id a_link_id, LinkQueueType a_type);
    ~LinkQueue() = default;

    bool push(Packet packet) final;
    Packet front() final;
    void pop() final;

    Size get_size() const final;
    bool empty() const final;
    Size get_max_size() const final;

private:
    SimplePacketQueue m_queue;
    Id m_link_id;
    LinkQueueType m_type;
};
}  // namespace sim