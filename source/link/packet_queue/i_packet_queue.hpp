#pragma once
#include "packet.hpp"
#include "types.hpp"

namespace sim {
class IPacketQueue {
public:
    virtual ~IPacketQueue() = default;

    virtual bool push(Packet packet) = 0;
    virtual Packet front() const = 0;
    virtual void pop() = 0;

    virtual bool empty() const = 0;
    virtual SizeByte get_size() const = 0;
    virtual SizeByte get_max_size() const = 0;
};
}  // namespace sim