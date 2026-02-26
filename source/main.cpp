#include <cxxopts.hpp>

#include "logger/logger.hpp"
#include "metrics/metrics_collector.hpp"
#include "parser/new_scenario/new_scenario_parser.hpp"
#include "parser/parser.hpp"
#include "utils/statistics.hpp"
#include "utils/summary.hpp"

int main(const int argc, char** argv) {
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
        "metrics-filter", "Filter for collecting metrics pathes",
        cxxopts::value<std::string>()->default_value(".*"))(
        "use-new-simulator", "Use new acritecture or old one",
        cxxopts::value<bool>()->default_value("false"))("h,help",
                                                        "Print usage");

    auto flags = options.parse(argc, argv);
    auto output_dir = flags["output-dir"].as<std::string>();
    Logger::set_output_dir(output_dir);

    if (flags.contains("help")) {
        std::cout << options.help() << std::endl;
        return 0;
    }

    if (flags["no-logs"].as<bool>()) {
        Logger::get_instance().disable_logs();
    }

    std::filesystem::path config_path = flags["config"].as<std::string>();

    std::filesystem::path actions_summary_path(
        std::filesystem::path(output_dir) / "actions_summary.csv");

    if (flags["use-new-simulator"].as<bool>()) {
        sim::NewScenario scenario = sim::parse_scenario(config_path);
        const sim::NewSummary& summary = scenario.simulate();

        sim::write_to_csv(actions_summary_path, summary.send_data);
        return 0;
    }

    sim::MetricsCollector::set_metrics_filter(
        flags["metrics-filter"].as<std::string>());

    sim::YamlParser parser;
    sim::Simulator simulator = parser.build_simulator_from_config(config_path);

    simulator.start();

    if (!flags["no-plots"].as<bool>()) {
        sim::MetricsCollector::get_instance().draw_metric_plots(output_dir);
    }
    sim::MetricsCollector::get_instance().export_metrics_to_files(output_dir);

    std::filesystem::path summary_path(std::filesystem::path(output_dir) /
                                       "summary.csv");

    sim::Summary summary = sim::Summary(simulator.get_connections());

    summary.write_to_csv(summary_path);
    summary.check();

    std::shared_ptr<sim::SendDataActionsSummary> send_data_summary =
        simulator.get_scenario().get_summary();

    sim::write_to_csv(actions_summary_path, *send_data_summary.get());

    return 0;
}
