#include "NetworkSimulator.hpp"
#include "Flow.hpp"

void NetworkSimulator::Run() {
    // Send packets to switch
    for (std::unique_ptr<Flow> const& flow : flows) {
        flow->Send();
    }

}
