#pragma once
#include "device/interfaces/i_host.hpp"
#include "link/i_link.hpp"
#include "packet.hpp"

namespace sim {
struct Endpoints {
    std::shared_ptr<IHost> sender;
    std::shared_ptr<IHost> receiver;

    Endpoints(std::shared_ptr<IHost> a_sender,
              std::shared_ptr<IHost> a_receiver)
        : sender(a_sender), receiver(a_receiver) {}
};
}  // namespace sim