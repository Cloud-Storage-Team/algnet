#include <string>
#include <variant>
#include <exception>

#include "logger/logger.hpp"
#include "metrics/metrics_collector.hpp"
#include "parser/parser.hpp"
#include "spdlog/fmt/bundled/format.h"

int main(const int argc, char **argv) {
    std::string output_dir = "";
    if (argc < 2) {
        LOG_ERROR(
            fmt::format("Usage: {} <config.yaml> [output-dir] "
                        "[--export-metrics] [--no-plots] [--no-logs]",
                        argv[0]));
        return 1;
    } else if (argc > 2) {
        output_dir = argv[2];
    }

    bool export_metrics_flag = false;
    bool draw_plots_flag = true;

    for (auto i = 2; i < argc; ++i) {
        if (std::string(argv[i]) == "--export-metrics") {
            export_metrics_flag = true;
        } else if (std::string(argv[i]) == "--no-plots") {
            draw_plots_flag = false;
        } else if (std::string(argv[i]) == "--no-logs") {
            Logger::get_instance().disable_logs();
        }
    }

    try {
        sim::YamlParser parser;
        auto [simulator, simulation_time] =
            parser.build_simulator_from_config(argv[1]);
        std::visit([&](auto &sim) { sim.start(simulation_time); }, simulator);

        if (draw_plots_flag) {
            sim::MetricsCollector::get_instance().draw_metric_plots(output_dir);
        }
        if (export_metrics_flag) {
            sim::MetricsCollector::get_instance().export_metrics_to_files(
                output_dir);
        }
    } catch (const std::exception &e) {
        LOG_ERROR(fmt::format("Error: {}", e.what()));
        return 1;
    }
    return 0;
}
