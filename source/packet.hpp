#pragma once
#include <cstdint>

<<<<<<< HEAD
namespace sim {

class Device;
class Flow;
=======
#include "device.hpp"
#include "flow.hpp"

namespace sim {
>>>>>>> origin/main

enum PacketType { ACK, DATA };

struct Packet {
    PacketType type;
    std::uint32_t size;
    Flow* flow;
};

}  // namespace sim
