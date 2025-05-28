#include <yaml-cpp/yaml.h>

#include "source/logger/logger.hpp"
#include "source/parser/parser.hpp"
#include "source/simulator.hpp"

int main(const int argc, char **argv) {
    if (argc < 2) {
        LOG_ERROR(
            fmt::format("Usage: {} <config.yaml> [output-dir] [--export-metrics] [--no-plots]",
                        argv[0]));
        return 1;
    } else if (argc > 2) {
        sim::MetricsCollector::init(argv[2]);
    }

    bool export_metrics_flag = false;
    bool draw_plots_flag = true;
    
    for (auto i = 2; i < argc; ++i) {
        if (std::string(argv[i]) == "--export-metrics") {
            export_metrics_flag = true;
        } else if (std::string(argv[i]) == "--no-plots") {
            draw_plots_flag = false;
        }
    }

    try {
        sim::YamlParser parser;
        auto [simulator, simulation_time] =
            parser.build_simulator_from_config(argv[1]);
        std::visit(
            [&](auto &sim) {
                sim.start(simulation_time, export_metrics_flag,
                          draw_plots_flag);
            },
            simulator);
    } catch (const std::exception &e) {
        LOG_ERROR(fmt::format("Error: {}", e.what()));
        return 1;
    }
    return 0;
}
