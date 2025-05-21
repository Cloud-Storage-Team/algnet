#include <yaml-cpp/yaml.h>

#include "parser.hpp"
#include "source/logger/logger.hpp"
#include "source/parser.hpp"
#include "source/simulator.hpp"

int main(const int argc, char **argv) {
    if (argc != 2) {
        LOG_ERROR(fmt::format("Usage: {} <config.yaml>", argv[0]));
        return 1;
    }

    constexpr int sim_time = 100'000;

    try {
        sim::YamlParser parser;
        auto [simulator, _] =
            parser.build_simulator_from_config(argv[1]);
        std::visit([&](auto &sim) { sim.start(sim_time); }, simulator);
    } catch (const std::exception &e) {
        LOG_ERROR(fmt::format("Error: {}", e.what()));
        return 1;
    }
    return 0;
}
