import sys
import os
import subprocess

def check_directory(dirname : str):
    if not os.path.isdir(dirname):
        os.mkdir(dirname)

def main(args):
    assert(len(args) >= 4)
    simulator_path = args[1]
    simulation_configs_dif = args[2]
    corner_metrics_dir = args[3]

    check_directory(corner_metrics_dir)

    for filename in os.listdir(simulation_configs_dif):
        if not filename.endswith('.yml'):
            continue
        no_extention_name = os.path.splitext(filename)[0]

        metrics_dir = os.path.join(corner_metrics_dir, no_extention_name)
        check_directory(metrics_dir)

        filepath = os.path.join(simulation_configs_dif, filename)
        print(f"Run {simulator_path} {filepath} {metrics_dir}")

        subprocess.run([simulator_path, filepath, metrics_dir])


if __name__ == "__main__":
    main(sys.argv)
