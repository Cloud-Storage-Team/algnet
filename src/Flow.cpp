#include "Flow.hpp"

#include <stdexcept>

Flow::Flow(const std::vector<std::shared_ptr<NetworkDevice>>& path, const std::vector<std::uint32_t>& distances_ns) {
    if (path.size() < 2) {
        throw std::runtime_error("Error: Incorrect flow path.");
    }
    if (distances_ns.size() != path.size() - 1) {
        throw std::runtime_error("Error: Incorrect flow distances.");
    }
    this->path = path;
    this->distances_ns = distances_ns;
    id = Flow::last_given_flow_id++;
}

void Flow::Send() {
    std::shared_ptr<NetworkDevice> server = path.at(0);
    server->Send(id);
}

std::pair<std::uint32_t, std::uint32_t> Flow::FindNextDeviceInPath(std::uint32_t sender_id) {
    for (std::uint32_t i = 1; i < path.size(); ++i) {
        if (path[i - 1]->id == sender_id) {
            return {path[i]->id, distances_ns[i - 1]};
        }
    }
    return {0, 0};
}