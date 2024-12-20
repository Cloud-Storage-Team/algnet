#include <memory>
#include <stdexcept>

#include "Flow.hpp"
#include "NetworkSimulator.hpp"

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
    std::shared_ptr<NetworkDevice> sender = path.front();
    std::shared_ptr<NetworkDevice> receiver = path.back();
    Packet p(sender->id, receiver->id, 0, false);

    NetworkSimulator::Schedule(packet_generation_interval_ns, [this, sender, p]() {
        sender->ProcessPacket(p);
        Send();
    });
}