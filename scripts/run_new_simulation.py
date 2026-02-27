import sys
import os
import subprocess
import yaml
import shutil
import argparse


def check_directory(dirname: str):
    if not os.path.isdir(dirname):
        os.mkdir(dirname)

def get_network_path(scenario_config_path: str):
    with open(scenario_config_path, "r") as f:
        config = yaml.safe_load(f)
    config_file_dirname = os.path.dirname(scenario_config_path)
    relative_network_path = config.get("network_config_path", "")
    network_path = os.path.join(config_file_dirname, relative_network_path)
    if not network_path:
        sys.exit("Error: No topology_config_path found in the configuration file.")
    return network_path

def get_topology_path(simulation_config_path: str):
    with open(simulation_config_path, "r") as f:
        config = yaml.safe_load(f)
    config_file_dirname = os.path.dirname(simulation_config_path)
    relative_topology_path = config.get("topology_config_path", "")
    topology_path = os.path.join(config_file_dirname, relative_topology_path)
    if not topology_path:
        sys.exit("Error: No topology_config_path found in the configuration file.")
    return topology_path

def get_absolute_paths(file_path):
    base_dir = os.path.dirname(os.path.abspath(file_path))

    absolute_paths = []

    with open(file_path, 'r') as file:
        for line in file:
            relative_path = line.strip()
            if relative_path:
                absolute_paths.append(os.path.join(base_dir, relative_path))

    return absolute_paths


def copy_topology_image(topology_name: str, metrics_dir: str):
    image_path = os.path.join(
        "configuration_examples", "generator", "topology_images", f"{topology_name}.svg"
    )

    if os.path.isfile(image_path):
        shutil.copy2(image_path, metrics_dir)
    else:
        print(f"Warning: Topology image not found at {image_path}")


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

    check_directory(corner_metrics_dir)

    for scenario_config_path in get_absolute_paths(scenario_configs_path):
        print(f"Scnerio config path: {scenario_config_path}")
        if not scenario_config_path.endswith(".yml") and not scenario_config_path.endswith(".yaml"):
            continue

        no_extension_name = os.path.splitext(scenario_config_path)[0]
        metrics_dir = os.path.join(corner_metrics_dir, no_extension_name)
        # TODO: fix metrics dir calculation & remove exit(-1)
        check_directory(metrics_dir)
        print(f"Metrics dir: {metrics_dir}")
        print(f"corner_metrics_dir: {corner_metrics_dir}")
        print(f"no_extension_name: {no_extension_name}")
        exit(-1)

        simulation_config_filepath = scenario_config_path

        current_file_path = os.path.abspath(__file__)
        image_generator_path =  os.path.join(os.path.dirname(current_file_path), "generate_image.py")
        output_image_path = os.path.join(metrics_dir, "topology.svg")

        network_path = get_network_path(scenario_config_path)

        topology_path = get_topology_path(network_path)

        image_generator_args = [
            "python3",
            image_generator_path,
            "-c",
            topology_path,
            "-o",
            output_image_path
        ]
        result = subprocess.run(image_generator_args, capture_output=True)
        if result.returncode != 0:
            print(f"Error running image generator.")
            print(f"SCript output: {result.stderr.decode()}")
            sys.exit(1)

        print(f"Run {simulator_path} {simulation_config_filepath} {metrics_dir}")

        simulator_args = [
            simulator_path,
            "--config",
            simulation_config_filepath,
            "--output-dir",
            metrics_dir,
            "--use-new-simulator"
        ]
        if parsed_args.no_logs:
            simulator_args.append("--no-logs")
        if parsed_args.no_plots:
            simulator_args.append("--no-plots")

        result = subprocess.run(simulator_args, capture_output=True)
        if result.returncode != 0:
            print(f"Error running nons on {simulation_config_filepath}.")
            print(f"Simulator output: {result.stderr.decode()}")
            sys.exit(1)


if __name__ == "__main__":
    main(sys.argv)
