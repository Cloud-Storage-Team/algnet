#include "NetworkSimulator.hpp"

#include <iostream>

NetworkSimulator::NetworkSimulator(const std::vector<std::uint32_t>& distances) {
    flow = std::make_unique<Flow>(distances);
}

void NetworkSimulator::Run() {
    flow->Send();
    std::uint32_t queue_size_packets = flow->GetPriorityQueueSize();
    std::cout << "Priority queue size: " << queue_size_packets << std::endl;
}
