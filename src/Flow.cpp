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

std::tuple<std::uint32_t, std::uint64_t, std::uint32_t, std::uint64_t> Flow::FindAdjDevicesInPath(std::uint32_t id) {
    for (std::uint32_t i = 1; i < path.size() - 1; ++i) {
        if (path[i]->id == id) {
            return {path[i - 1]->id, distances_ns[i - 1], path[i + 1]->id, distances_ns[i]};
        }
    }
    if (path[0]->id == id) {
        return {path[1]->id, distances_ns.front(), path[1]->id, distances_ns.front()};
    }
    if (path[path.size() - 1]->id == id) {
        return {path[path.size() - 2]->id, distances_ns.back(), path[path.size() - 2]->id, distances_ns.back()};
    }
    return {0, 0, 0, 0};
}