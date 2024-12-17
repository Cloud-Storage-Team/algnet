#include "Flow.hpp"
#include "NetworkSimulator.hpp"

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

void Flow::Send(double delta_time_ns) {
    std::shared_ptr<NetworkDevice> sender = path.front();
    std::shared_ptr<NetworkDevice> receiver = path.back();

    double current_time_ns = 0;
    while (current_time_ns < 1e9) {
        Packet p(sender->id, receiver->id, 0, false);
        NetworkSimulator::Schedule(current_time_ns, [&]() {
            sender->Enqueue(p);
            double delay_ns = 1.0;
            NetworkSimulator::Schedule(delay_ns, [&](){ sender->ProcessPacket(); });
        });
        current_time_ns += delta_time_ns;
    }
}