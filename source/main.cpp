#include <cxxopts.hpp>

#include "logger/logger.hpp"
#include "parser/new_scenario/new_scenario_parser.hpp"
#include "utils/statistics.hpp"

int main(const int argc, char** argv) {
    cxxopts::Options options("NoNS", "Discrete-event based simulator");
    options.add_options()("c,config",
                          "Path to the simulation configuration file",
                          cxxopts::value<std::string>())(
        "output-dir", "Output directory for metrics and plots",
        cxxopts::value<std::string>()->default_value("metrics"))(
        "no-logs", "Output without logs",
        cxxopts::value<bool>()->default_value("false"))(
        "metrics-filter", "Filter for collecting metrics pathes",
        cxxopts::value<std::string>()->default_value(".*"))("h,help",
                                                            "Print usage");

    auto flags = options.parse(argc, argv);
    std::filesystem::path output_dir = flags["output-dir"].as<std::string>();
    Logger::set_output_dir(output_dir);

    if (flags.contains("help")) {
        std::cout << options.help() << std::endl;
        return 0;
    }

    if (flags["no-logs"].as<bool>()) {
        Logger::get_instance().disable_logs();
    }

    std::filesystem::path config_path = flags["config"].as<std::string>();

    std::filesystem::path actions_summary_path(output_dir /
                                               "actions_summary.csv");

    sim::NewScenario scenario = sim::parse_scenario(config_path);
    sim::NewSummary summary = scenario.simulate();

    sim::write_to_csv(actions_summary_path, summary.send_data);
    scenario.get_network().save_metrics(output_dir / "network");

    return 0;
}
