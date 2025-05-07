#include <yaml-cpp/yaml.h>

#include <variant>

#include "parser.hpp"
#include "source/logger/logger.hpp"
#include "source/parser.hpp"
#include "source/simulator.hpp"

int main(const int argc, char **argv) {
    if (argc != 2) {
        LOG_ERROR(fmt::format("Usage: {} <config.yaml>", argv[0]));
        return 1;
    }

    try {
        sim::YamlParser parser;
        sim::SimulatorVariant simulator =
            parser.buildSimulatorFromConfig(argv[1]);
        std::visit([&](auto &sim) { sim.start(parser.get_simulation_time()); },
                   simulator);
    } catch (const std::exception &e) {
        LOG_ERROR(fmt::format("Error: {}", e.what()));
        return 1;
    }
    return 0;
}
