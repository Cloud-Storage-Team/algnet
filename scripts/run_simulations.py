import sys
import os
import subprocess


def check_directory(dirname: str):
    if not os.path.isdir(dirname):
        os.mkdir(dirname)


def separate_files(metrics_dir: str):
    link_dir = os.path.join(metrics_dir, "link")
    flow_dir = os.path.join(metrics_dir, "flow")
    check_directory(link_dir)
    check_directory(flow_dir)

    for filename in os.listdir(metrics_dir):
        filepath = os.path.join(metrics_dir, filename)
        if os.path.isfile(filepath):
            if "link" in filename:
                os.rename(filepath, os.path.join(link_dir, filename))
            elif "flow" in filename:
                os.rename(filepath, os.path.join(flow_dir, filename))


def main(args):
    assert len(args) >= 4
    simulator_path = args[1]
    simulation_configs_dif = args[2]
    corner_metrics_dir = args[3]

    check_directory(corner_metrics_dir)

    for filename in os.listdir(simulation_configs_dif):
        if not filename.endswith(".yml") and not filename.endswith(".yaml"):
            continue
        no_extention_name = os.path.splitext(filename)[0]

        metrics_dir = os.path.join(corner_metrics_dir, no_extention_name)
        check_directory(metrics_dir)

        filepath = os.path.join(simulation_configs_dif, filename)
        print(f"Run {simulator_path} {filepath} {metrics_dir}")

        subprocess.run(
            [simulator_path, filepath, metrics_dir], stdout=subprocess.DEVNULL
        )

        separate_files(metrics_dir)


if __name__ == "__main__":
    main(sys.argv)
