#include "simple_packet_queue.hpp"

namespace sim {
SimplePacketQueue::SimplePacketQueue(Size a_max_size)
    : m_queue(), m_size(0), m_max_size(a_max_size) {}

bool SimplePacketQueue::push(Packet packet) {
    if (m_size + packet.size_byte > m_max_size) {
        return false;
    }
    m_size += packet.size_byte;
    m_queue.push(std::move(packet));
    return true;
}

Packet SimplePacketQueue::front() const {
    if (m_queue.empty()) {
        throw std::runtime_error("Can not get front packet from empty queue");
    }
    return m_queue.front();
}

void SimplePacketQueue::pop() {
    if (m_queue.empty()) {
        throw std::runtime_error("Can not pop packet from empty queue");
    }
    m_size -= m_queue.front().size_byte;
    m_queue.pop();
}

Size SimplePacketQueue::get_size() const { return m_size; }

bool SimplePacketQueue::empty() const { return m_queue.empty(); }

Size SimplePacketQueue::get_max_size() const { return m_max_size; }

}  // namespace sim