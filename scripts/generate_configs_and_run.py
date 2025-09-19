import sys
import os
import argparse
import tempfile
import subprocess

from common import *

def rel_from_cwd(path : str):
    # Returns relative version of path from current directory
    return os.path.relpath(path, os.getcwd())

def full_path_from_file(path : str, file = __file__):
    # build path from directory of file
    return os.path.join(os.path.dirname(file), path)

def rel_from_cwd_path_from_file(path : str, file = __file__):
    return rel_from_cwd(full_path_from_file(path, file))

def parse_args():
    parser = argparse.ArgumentParser(
        description="Generate topology, simulation and runs NoNS on them"
    )
    parser.add_argument(
        "-e", "--executable", help="Path to NoNS executable", required=True
    )
    all_to_all_script = rel_from_cwd_path_from_file(os.path.join("generators", "topology", "fat_tree", "fat_tree.py"))
    parser.add_argument(
        "-tg", "--topology_generator", help="Path to topology generator", default=all_to_all_script
    )
    parser.add_argument(
        "-tgc", "--topology_generator_config", help="Path to topology generator config"
    )
    all_to_all_script = rel_from_cwd_path_from_file(os.path.join("generators", "ti_simulation", "all-to-all", "all-to-all.py"))
    parser.add_argument(
        "-tisg", "--ti_simulation_generator", help="Path to topology independent simulation generator", default=all_to_all_script
    )
    parser.add_argument(
        "-tisgc", "--ti_simulation_generator_config", help="Path to simulation generator config"
    )
    return parser.parse_args()

def run_generator(generator_args : str, generator_config : str | None, output_file : str):
    # Adds config if present 
    if generator_config is not None:
        generator_args += ["-c", generator_config]
    
    result = run_subprocess(generator_args)
    if result.returncode != 0:
        exit(-1)

def main(args):
    args = parse_args()
    topology_genrator_script = args.topology_generator
    topology_generator_config = args.topology_generator_config

    simulation_genrator_script = args.ti_simulation_generator
    simulation_generator_config = args.ti_simulation_generator_config

    executable = args.executable

    with tempfile.NamedTemporaryFile(delete=True) as temp_topology_file:
        # Generates topology
        topology_generator_args = [
            "python3",
            
        ]
        run_generator(topology_genrator_script, topology_generator_config, temp_topology_file.name)
        with tempfile.NamedTemporaryFile(delete=True) as temp_simulation_file:
            # Generates topology
            run_generator(simulation_genrator_script, simulation_generator_config, temp_simulation_file.name)

            executable_args = {
                executable,
                "-c",
                temp_simulation_file,
            }
            result = run_subprocess(executable_args)
            exit(result.returncode)


if __name__ == "__main__":
    main(sys.argv)
