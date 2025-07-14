import sys
import os
import subprocess
import yaml
import shutil
import argparse


def check_directory(dirname: str):
    if not os.path.isdir(dirname):
        os.mkdir(dirname)


def get_topology_path(simulation_config_path: str):
    with open(simulation_config_path, "r") as f:
        config = yaml.safe_load(f)
    config_file_dirname = os.path.dirname(simulation_config_path)
    relative_topology_path = config.get("topology_config_path", "")
    topology_path = os.path.join(config_file_dirname, relative_topology_path)
    if not topology_path:
        sys.exit("Error: No topology_config_path found in the configuration file.")
    return topology_path


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
        "--config",
        help="Path to the directory with simulation configs",
        required=True,
    )
    parser.add_argument(
        "--output-dir", help="Directory to export simulation results", default="metrics"
    )
    parser.add_argument(
        "--export-metrics", action="store_true", help="Export simulation metrics"
    )
    parser.add_argument("--no-logs", action="store_true", help="Disable logging")
    parser.add_argument(
        "--no-plots", action="store_true", help="Disable plots generation"
    )

    parsed_args = parser.parse_args()

    simulator_path = parsed_args.executable
    simulation_configs_dir = parsed_args.config
    corner_metrics_dir = parsed_args.output_dir

    check_directory(corner_metrics_dir)

    for simulation_config_filename in os.listdir(simulation_configs_dir):
        if not simulation_config_filename.endswith(".yml") and not simulation_config_filename.endswith(".yaml"):
            continue

        no_extention_name = os.path.splitext(simulation_config_filename)[0]
        metrics_dir = os.path.join(corner_metrics_dir, no_extention_name)
        check_directory(metrics_dir)

        simulation_config_filepath = os.path.join(simulation_configs_dir, simulation_config_filename)

        current_file_path = os.path.abspath(__file__)
        image_generator_path =  os.path.join(os.path.dirname(current_file_path), "generate_image.py")
        output_image_path = os.path.join(metrics_dir, "topology.svg")

        topology_path = get_topology_path(simulation_config_filepath)

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
            exit(1)

        print(f"Run {simulator_path} {simulation_config_filepath} {metrics_dir}")

        simulator_args = [
            simulator_path,
            "--config",
            simulation_config_filepath,
            "--output-dir",
            metrics_dir,
        ]
        if parsed_args.export_metrics:
            simulator_args.append("--export-metrics")
        if parsed_args.no_logs:
            simulator_args.append("--no-logs")
        if parsed_args.no_plots:
            simulator_args.append("--no-plots")

        result = subprocess.run(simulator_args, capture_output=True)
        if result.returncode != 0:
            print(f"Error running nons on {simulation_config_filepath}.")
            print(f"Simulator output: {result.stderr.decode()}")
            exit(1)

        # separate_files(metrics_dir)


if __name__ == "__main__":
    main(sys.argv)
