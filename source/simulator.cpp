#include "simulator.hpp"

#include <stdexcept>
#include <string_view>
#include <string>

namespace sim {

SimulatorVariant create_simulator(std::string_view algorithm) {
    if (algorithm == "basic") {
        return BasicSimulator();
    }
    if (algorithm == "tcp") {
        return TcpSimulator();
    }
    throw std::invalid_argument("Unknown algorithm: " + std::string(algorithm));
}

}  // namespace sim
