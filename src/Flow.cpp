#include "Flow.hpp"
#include "Server.hpp"

Flow::Flow(const std::vector<std::uint32_t>& distances) : destination_node(0) {
    for (std::uint32_t distance : distances) {
        source_nodes.emplace_back(++last_given_id, distance);
    }
}

void Flow::Send() {
    for (ServerSender& s: source_nodes) {
        for (std::uint64_t i = 0; i < s.GetCWNDSize(); ++i) {
            packets.emplace(s.GetID(), s.GetCurrentTime(), s.GetCurrentTime() + s.GetDistance());
            s.IncreaseCurrentTime(s.GetDistance());
        }
    }
}
