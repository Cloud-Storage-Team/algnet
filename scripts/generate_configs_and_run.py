import os
import sys
import argparse
import tempfile
import time

from common import *
from generators.topology.common import *
from generators.ti_simulation.common import *

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

def main():
    args = parse_args()
    topology_generator_script = args.topology_generator
    topology_generator_config = args.topology_generator_config

    simulation_genrator_script = args.simulation_generator
    simulation_generator_config = args.simulation_generator_config

    executable = args.executable

    with tempfile.NamedTemporaryFile(delete=True) as temp_topology_file:
        # Generate topology
        generate_topology(topology_generator_script, temp_topology_file.name, topology_generator_config)
        with tempfile.NamedTemporaryFile(delete=True) as temp_simulation_file:
            # Generate topology
            generate_simulation_by_topology(
                simulation_genrator_script,
                temp_topology_file.name,
                temp_simulation_file.name,
                simulation_generator_config
            )

            # Run Nons
            start_time = time.perf_counter()
            result = run_nons_in_temp_dir(executable, temp_simulation_file.name)
            if (result.returncode != 0):
                exit(result.returncode)
            end_time = time.perf_counter()
            elapced = end_time - start_time
            print(f"Elapced {elapced:.3f} sec")
            


if __name__ == "__main__":
    main()
