#pragma once
#include <optional>
#include <queue>

#include "packet.hpp"
#include "types.hpp"

namespace sim {
class PacketQueue {
public:
    PacketQueue(Size a_max_size);

    // Adds packet to queue
    // returns true on succseed (remaining space is enought), false otherwice
    bool push(Packet packet);
    Packet front();
    void pop();

    Size get_size() const;
    bool empty() const;
    Size get_max_size() const;

private:
    std::queue<Packet> m_queue;
    Size m_size;
    Size m_max_size;
};
}  // namespace sim