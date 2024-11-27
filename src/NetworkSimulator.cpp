#include "NetworkSimulator.hpp"

NetworkSimulator::NetworkSimulator(const std::vector<std::uint32_t>& distances) {
    flow = new Flow(distances);
}

NetworkSimulator::~NetworkSimulator() {
    delete flow;
}

void NetworkSimulator::Run() {
    flow->Send();
    flow->PrintQueueSize();
}
