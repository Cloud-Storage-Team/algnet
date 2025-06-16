#include "simulator.hpp"

#include <stdexcept>

namespace sim {

SimulatorVariant create_simulator(std::string_view algorithm) {
    if (algorithm == "new_basic") {
        return BasicSimulator();
    }
    if (algorithm == "new_tcp") {
        return TcpSimulator();
    }
    throw std::invalid_argument("Unknown algorithm: " + std::string(algorithm));
}

}  // namespace sim
