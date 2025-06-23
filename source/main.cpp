#include <cxxopts.hpp>

#include "logger/logger.hpp"
#include "metrics/metrics_collector.hpp"
#include "parser/parser.hpp"

int main(const int argc, char **argv) {
    cxxopts::Options options("NoNS", "Discrete-event based simulator");
    options.add_options()("c,config",
                          "Path to the simulation configuration file",
                          cxxopts::value<std::string>())(
        "output-dir", "Output directory for metrics and plots",
        cxxopts::value<std::string>()->default_value("metrics"))(
        "no-logs", "Output without logs",
        cxxopts::value<bool>()->default_value("false"))(
        "no-plots", "Disables plots generation",
        cxxopts::value<bool>()->default_value("false"))(
        "export-metrics", "Export metric values into output-dir",
        cxxopts::value<bool>()->default_value("false"))("h,help",
                                                        "Print usage");

    try {
        auto flags = options.parse(argc, argv);
        auto output_dir = flags["output-dir"].as<std::string>();

        if (flags.contains("help")) {
            std::cout << options.help() << std::endl;
            return 0;
        }

        if (flags["no-logs"].as<bool>()) {
            Logger::get_instance().disable_logs();
        }

        sim::YamlParser parser;
        auto [simulator, simulation_time] = parser.build_simulator_from_config(
            flags["config"].as<std::string>());
        std::visit([&](auto &sim) { sim.start(simulation_time); }, simulator);

        if (!flags["no-plots"].as<bool>()) {
            sim::MetricsCollector::get_instance().draw_metric_plots(output_dir);
        }
        if (flags["export-metrics"].as<bool>()) {
            sim::MetricsCollector::get_instance().export_metrics_to_files(
                output_dir);
        }
    } catch (const std::exception &e) {
        std::cerr << fmt::format("Error: {}", e.what()) << std::endl;
        return 1;
    }
    return 0;
}
