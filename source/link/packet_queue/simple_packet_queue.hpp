#pragma once
#include <queue>

#include "i_packet_queue.hpp"

namespace sim {
class SimplePacketQueue : public IPacketQueue {
public:
    SimplePacketQueue(Size a_max_size);
    ~SimplePacketQueue() = default;

    // Adds packet to queue
    // returns true on succseed (remaining space is enought), false
    // otherwice
    bool push(Packet packet) final;
    Packet front() const final;
    void pop() final;

    Size get_size() const final;
    bool empty() const final;
    Size get_max_size() const final;

private:
    std::queue<Packet> m_queue;
    Size m_size;
    Size m_max_size;
};
}  // namespace sim