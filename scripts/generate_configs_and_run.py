import os
import sys
import argparse
import tempfile

from common import *
from generators.topology.common import TOPOLOGY_GENERATORS_DIR
from generators.ti_simulation.common import TI_SIMULATION_GENERATORS_DIR

def parse_args():
    parser = argparse.ArgumentParser(
        description="Generate topology, simulation and runs executable (NoNS) on them"
    )
    parser.add_argument(
        "-e", "--executable", help="Path to NoNS executable", required=True
    )
    fat_tree_script = os.path.join(TOPOLOGY_GENERATORS_DIR, "fat_tree", "fat_tree.py")
    parser.add_argument(
        "-t", "--topology_generator", help="Path to topology generator", default=fat_tree_script
    )
    parser.add_argument(
        "-tc", "--topology_generator_config", help="Path to topology generator config"
    )
    all_to_all_script = os.path.join(TI_SIMULATION_GENERATORS_DIR, "all-to-all", "all-to-all.py")
    parser.add_argument(
        "-s", "--simulation_generator", help="Path to topology independent simulation generator", default=all_to_all_script
    )
    parser.add_argument(
        "-sc", "--simulation_generator_config", help="Path to simulation generator config"
    )
    return parser.parse_args()

def run_generator(generator_sript : str, generator_config : str | None, output_file : str) -> int:
    """
    Run generator_script on generator config (or use default if config is None),
    puts result ti output_file. Returns result of subprocess run
    """
    args = ["python3", generator_sript, "-o", output_file]
    if generator_config is not None:
        args += ["-c", generator_config]
    
    result = run_subprocess(args)
    return result.returncode

def main():
    args = parse_args()
    topology_generator_script = args.topology_generator
    topology_generator_config = args.topology_generator_config

    simulation_genrator_script = args.simulation_generator
    simulation_generator_config = args.simulation_generator_config

    executable = args.executable

    with tempfile.NamedTemporaryFile(delete=True) as temp_topology_file:
        # Generate topology
        run_generator(topology_generator_script, topology_generator_config, temp_topology_file.name)
        with tempfile.NamedTemporaryFile(delete=True) as temp_simulation_file:
            # Generate topology
            run_generator(simulation_genrator_script, simulation_generator_config, temp_simulation_file.name)

            executable_args = {
                executable,
                "-c",
                temp_simulation_file,
            }
            # Run executale
            result = run_subprocess(executable_args)
            sys.exit(result.returncode)


if __name__ == "__main__":
    main()
