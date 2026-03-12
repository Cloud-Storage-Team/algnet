#pragma once
#include "network/connection/flow/packet.hpp"
#include "topology/device/interfaces/i_host.hpp"
#include "topology/link/i_link.hpp"

namespace sim {
struct Endpoints {
    std::shared_ptr<IHost> sender;
    std::shared_ptr<IHost> receiver;

    Endpoints(std::shared_ptr<IHost> a_sender,
              std::shared_ptr<IHost> a_receiver)
        : sender(a_sender), receiver(a_receiver) {}
};
}  // namespace sim
