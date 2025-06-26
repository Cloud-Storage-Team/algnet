#include "packet_queue.hpp"

namespace sim {
PacketQueue::PacketQueue(Size a_max_size)
    : m_queue(), m_size(0), m_max_size(a_max_size) {}

bool PacketQueue::push(Packet packet) {
    if (m_size + packet.size_byte > m_max_size) {
        return false;
    }
    m_size += packet.size_byte;
    m_queue.push(std::move(packet));
    return true;
}

Packet PacketQueue::front() {
    if (m_queue.empty()) {
        throw std::runtime_error("Can not get front packet from empty queue");
    }
    return m_queue.front();
}

void PacketQueue::pop() {
    if (m_queue.empty()) {
        throw std::runtime_error("Can not pop packet from empty queue");
    }
    m_size -= m_queue.front().size_byte;
    m_queue.pop();
}

Size PacketQueue::get_size() const { return m_size; }

bool PacketQueue::empty() const { return m_queue.empty(); }

Size PacketQueue::get_max_size() const { return m_max_size; }

}  // namespace sim