#include "NetworkSimulator.hpp"
#include "Flow.hpp"

#include <iostream>

NetworkSimulator::NetworkSimulator(const std::vector<std::uint32_t>& distances):
    flow(std::make_unique<Flow>(distances)) { }

void NetworkSimulator::Run() {
    flow->Send();
    std::cout << "Priority queue size: " << (*flow).packets.size() << std::endl;
}
