#include "Flow.hpp"

#include <stdexcept>

Flow::Flow(const std::vector<std::shared_ptr<NetworkDevice>>& path) {
    if (path.size() < 2) {
        throw std::runtime_error("Incorrect flow path.");
    }
    this->path = path;
}

void Flow::Send() {
    std::shared_ptr<NetworkDevice> server = path.at(0);
    std::shared_ptr<NetworkDevice> device_receiver = path.at(1);
    server->Send(device_receiver, distances_ns.at(0));
}
