import sys
import os
import yaml
import argparse

from common import run_subprocess

def get_network_path(scenario_config_path: str):
    with open(scenario_config_path, "r") as f:
        config = yaml.safe_load(f)
    config_file_dirname = os.path.dirname(scenario_config_path)
    relative_network_path = config.get("network_config_path", "")
    if not relative_network_path:
        sys.exit("Error: No topology_config_path found in the configuration file.")
    network_path = os.path.join(config_file_dirname, relative_network_path)
    return network_path

def get_topology_path(simulation_config_path: str):
    with open(simulation_config_path, "r") as f:
        config = yaml.safe_load(f)
    config_file_dirname = os.path.dirname(simulation_config_path)
    relative_topology_path = config.get("topology_config_path", "")
    if not relative_topology_path:
        sys.exit("Error: No topology_config_path found in the configuration file.")
    return os.path.join(config_file_dirname, relative_topology_path)

def get_relative_paths(scenario_configs_file):
    result = []
    with open(scenario_configs_file, 'r') as file:
        for line in file:
            relative_path = line.strip()
            if relative_path:
                result.append(relative_path)

    return result

def main(args):
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-e", "--executable", help="Path to the compiled project", required=True
    )
    parser.add_argument(
        "-c",
        "--configs",
        help="Path to the file with list of scenario configs",
        required=True,
    )
    parser.add_argument(
        "--output-dir", help="Directory to export simulation results", default="metrics"
    )
    parser.add_argument("--no-logs", action="store_true", help="Disable logging")
    parser.add_argument(
        "--no-plots", action="store_true", help="Disable plots generation"
    )

    parsed_args = parser.parse_args()

    simulator_path = parsed_args.executable
    scenario_configs_path = parsed_args.configs
    corner_metrics_dir = parsed_args.output_dir

    paths_base = os.path.dirname(os.path.abspath(scenario_configs_path))

    for rel_scenario_config_path in get_relative_paths(scenario_configs_path):
        print(f"Scnerio config path: {rel_scenario_config_path}")
        if not rel_scenario_config_path.endswith(".yml") and not rel_scenario_config_path.endswith(".yaml"):
            continue

        abs_scenario_config_path = os.path.join(paths_base, rel_scenario_config_path)

        no_extension_name = os.path.splitext(rel_scenario_config_path)[0]
        metrics_dir = os.path.join(corner_metrics_dir, no_extension_name)

        current_file_path = os.path.abspath(__file__)
        image_generator_path =  os.path.join(os.path.dirname(current_file_path), "generate_image.py")
        output_image_path = os.path.join(metrics_dir, "topology.svg")

        network_path = get_network_path(abs_scenario_config_path)

        topology_path = get_topology_path(network_path)

        image_generator_args = [
            "python3",
            image_generator_path,
            "-c",
            topology_path,
            "-o",
            output_image_path
        ]
        result = run_subprocess(image_generator_args)
        if result.returncode != 0:
            sys.exit(1)

        simulator_args = [
            simulator_path,
            "--config",
            abs_scenario_config_path,
            "--output-dir",
            metrics_dir,
            "--use-new-simulator"
        ]
        if parsed_args.no_logs:
            simulator_args.append("--no-logs")
        if parsed_args.no_plots:
            simulator_args.append("--no-plots")

        result = run_subprocess(simulator_args)
        if result.returncode != 0:
            sys.exit(1)


if __name__ == "__main__":
    main(sys.argv)
