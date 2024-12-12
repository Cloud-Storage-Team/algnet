#include "NetworkDevice.hpp"

#include <memory>

void Flow::Send() {
    std::shared_ptr<NetworkDevice> server = path.at(0);
    server->Send(id);
}