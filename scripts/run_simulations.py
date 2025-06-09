import sys
import os
import subprocess
import yaml
import shutil
import argparse


def check_directory(dirname: str):
    if not os.path.isdir(dirname):
        os.mkdir(dirname)


def get_topology_name(config_path: str):
    with open(config_path, "r") as f:
        config = yaml.safe_load(f)

    topology_path = config.get("topology_config_path", "")
    if not topology_path:
        sys.exit("Error: No topology_config_path found in the configuration file.")

    topology_filename = os.path.basename(topology_path)
    topology_name = os.path.splitext(topology_filename)[0]
    return topology_name


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
        "-s", "--simulator", help="Path to the simulator executable", required=True
    )
    parser.add_argument(
        "-c",
        "--config",
        help="Path to the simulation configuration file",
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

    simulator_path = parsed_args.simulator
    simulation_configs_dir = parsed_args.config
    corner_metrics_dir = parsed_args.output_dir

    check_directory(corner_metrics_dir)

    for filename in os.listdir(simulation_configs_dir):
        if not filename.endswith(".yml") and not filename.endswith(".yaml"):
            continue

        no_extention_name = os.path.splitext(filename)[0]
        metrics_dir = os.path.join(corner_metrics_dir, no_extention_name)
        check_directory(metrics_dir)

        filepath = os.path.join(simulation_configs_dir, filename)

        topology_name = get_topology_name(filepath)
        copy_topology_image(topology_name, metrics_dir)

        print(f"Run {simulator_path} {filepath} {metrics_dir}")

        simulator_args = [
            simulator_path,
            "--config",
            filepath,
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
            print(
                f"Error running simulator on {filepath}.\nSimulator output: {result.stderr.decode()}"
            )
            exit(1)

        # separate_files(metrics_dir)


if __name__ == "__main__":
    main(sys.argv)
