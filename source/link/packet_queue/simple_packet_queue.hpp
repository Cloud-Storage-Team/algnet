#pragma once
#include <optional>
#include <queue>

#include "i_packet_queue.hpp"

namespace sim {
class SimplePacketQueue : public IPacketQueue {
public:
    SimplePacketQueue(SizeByte a_max_size);
    ~SimplePacketQueue() = default;

    // Adds packet to queue
    // returns true on succseed (remaining space is enought), false
    // otherwice
    bool push(Packet packet) final;
    Packet front() final;
    void pop() final;

    SizeByte get_size() const final;
    bool empty() const final;
    SizeByte get_max_size() const final;

private:
    std::queue<Packet> m_queue;
    SizeByte m_size;
    SizeByte m_max_size;
};
}  // namespace sim