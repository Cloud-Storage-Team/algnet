#pragma once
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
    virtual bool push(const Packet& packet) final;
    virtual const Packet& front() const final;
    virtual Packet& front() final;
    virtual void pop() final;

    virtual SizeByte get_size() const final;
    virtual bool empty() const final;
    virtual SizeByte get_max_size() const final;

private:
    std::queue<Packet> m_queue;
    SizeByte m_size;
    SizeByte m_max_size;
};
}  // namespace sim