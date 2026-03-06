#pragma once
#include <cstdint>
namespace sim {

using Port = std::uint32_t;

struct EndpointPorts {
    Port sender_port = 0;
    Port receriver_port = 0;

    EndpointPorts(Port a_sender_port = 0, Port a_receiver_port = 0)
        : sender_port(a_sender_port), receriver_port(a_receiver_port) {}
};

}  // namespace sim
