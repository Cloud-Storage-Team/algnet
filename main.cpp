#include "source/logger/logger.hpp"
#include "source/parser.hpp"

int main(int argc, char* argv[]) {
    Logger::setupLogging();
    if (argc != 3) {
        LOG_ERROR(fmt::format("Usage: {} <topology.yaml> <simulation.yaml>", argv[0]));
        return 1;
    }

    try {
        sim::Simulator simulator;
        sim::YamlParser parser;
        Time stop_time = parser.parse_configs(argv[1], argv[2], simulator);
        simulator.start(stop_time);
    } catch (const std::exception& e) {
        LOG_ERROR(fmt::format("Error: {}", e.what()));
        return 1;
    }

    return 0;
}
