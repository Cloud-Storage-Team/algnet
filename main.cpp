#include <yaml-cpp/yaml.h>

#include "parser.hpp"
#include "source/logger/logger.hpp"
#include "source/parser.hpp"
#include "source/simulator.hpp"

int main(const int argc, char **argv) {
    LOG_INFO("Hello, World!");
    if (argc != 2) {
        LOG_ERROR(fmt::format("Usage: {} <config.yaml>", argv[0]));
        return 1;
    }

    try {
        sim::YamlParser parser;
        sim::BasicSimulator simulator = parser.buildSimulatorFromConfig(argv[1]);
        simulator.start(1000);  // Run simulation for 1000 time units
    } catch (const std::exception &e) {
        LOG_ERROR(fmt::format("Error: {}", e.what()));
        return 1;
    }
    return 0;
}
