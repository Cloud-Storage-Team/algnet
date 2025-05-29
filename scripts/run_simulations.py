import sys
import os
import subprocess
import yaml
import shutil


def check_directory(dirname: str):
    if not os.path.isdir(dirname):
        os.mkdir(dirname)


def separate_files(metrics_dir: str):
    link_dir = os.path.join(metrics_dir, "link")
    flow_dir = os.path.join(metrics_dir, "flow")
    cwnd_dir = os.path.join(metrics_dir, "cwnd")
    check_directory(link_dir)
    check_directory(flow_dir)
    check_directory(cwnd_dir)

    for filename in os.listdir(metrics_dir):
        filepath = os.path.join(metrics_dir, filename)
        if os.path.isfile(filepath):
            if "link" in filename:
                os.rename(filepath, os.path.join(link_dir, filename))
            elif "flow" in filename:
                os.rename(filepath, os.path.join(flow_dir, filename))
            elif "cwnd" in filename:
                os.rename(filepath, os.path.join(cwnd_dir, filename))


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
    assert len(args) >= 4
    simulator_path = args[1]
    simulation_configs_dir = args[2]
    corner_metrics_dir = args[3]

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

        subprocess.run(
            [simulator_path, filepath, metrics_dir], stdout=subprocess.DEVNULL
        )

        # separate_files(metrics_dir)


if __name__ == "__main__":
    main(sys.argv)
