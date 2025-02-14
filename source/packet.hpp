#pragma once

namespace sim {

class Device;
class Flow;

enum PacketType { ACK, DATA };

struct Packet {
    PacketType type;
    int size;
    Device* src;
    Device* dest;
    Flow* flow;
};

}  // namespace sim
