#include "simulator.hpp"

#include <stdexcept>

namespace sim {

SimulatorVariant create_simulator(std::string_view algorithm) {
    if (algorithm == "basic") {
        return BasicSimulator();
    }
    throw std::invalid_argument("Unknown algorithm: " + std::string(algorithm));
}

}  // namespace sim
