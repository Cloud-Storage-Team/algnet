#include "Flow.hpp"
#include "Server.hpp"

#include <iostream>

Flow::Flow(const std::vector<std::uint32_t>& distances) {
    destination_node = ServerBase(0);
    for (std::uint32_t distance : distances) {
        source_nodes.emplace_back(++last_given_server_id, distance);
    }
}

void Flow::Send() {
    for (auto& s: source_nodes) {
        for (std::uint64_t i = 0; i < s.GetCWNDSize(); ++i) {
            packets.emplace(s.GetID(), s.GetCurrentTime(), s.GetCurrentTime() + s.GetDistance());
            s.IncreaseCurrentTime(s.GetDistance());
        }
    }
}

void Flow::PrintQueueSize() const {
    std::cout << "Priority queue size: " << packets.size() << ".\n";

}

const std::vector<ServerSender>& Flow::GetSourceNodes() const {
    return source_nodes;
}

const ServerBase& Flow::GetDestinationNode() const {
    return destination_node;
}
