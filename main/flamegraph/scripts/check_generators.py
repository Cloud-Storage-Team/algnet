import argparse
import os
import tempfile
import sys

from common import *
from generators.simulation.common import *
from generators.ti_simulation.common import *
from generators.topology.common import *

def get_generator_dirs(generators_dir : str) -> list[str]:
    """
    Returns list of generators (all directories excecpt __pycache__) in generators_dir
    """
    return list(
        filter(
            lambda dir_path : os.path.isdir(dir_path) and os.path.basename(dir_path) != "__pycache__",
            map(
                lambda dirname : os.path.join(generators_dir, dirname),
                sorted(os.listdir(generators_dir))
            )
        )
    )

def find_generator_script(generator_dir : str) -> str | None:
    """
    Searches for generator (python script different from __init__.py) in generator_dir
    If there are suitable scripts or more that one, outs erro to stderr and retuns None
    Otherwise returns path to generator
    """
    python_scripts = list(
        filter(
            lambda path : path.endswith(".py") and os.path.basename(path) != "__init__.py",
            map(
                lambda item : os.path.join(generator_dir, item),
                os.listdir(generator_dir)
            )
        )
    )
    if len(python_scripts) == 1:
        return python_scripts[0]
    print(f"Found following python scripts in {generator_dir}: {python_scripts}; expected only one", file=sys.stderr)
    return None

def get_generators_scripts(generators_dir : str) -> list[str]:
    """
    Return list of generator scripts in generators_dir
    """
    return list(
        filter(
            lambda item : item is not None,
            map(
                find_generator_script,
                get_generator_dirs(generators_dir),
            )
        )
    )

def get_topology_config_path(simulation_config_path : str) -> str | None:
    simulation_config = load_yaml(simulation_config_path)
    return simulation_config.get("topology_config_path", None)


def check_simulation_generator(nons_path : str, simulation_generator_script : str) -> bool:
    """
    Checks given simulation generator:
    1) Generate simulation using simulation_generator_script
    2) Runs nons on it
    Returns true if check succeed, false otherwise
    """
    with tempfile.NamedTemporaryFile(delete=True) as temp_simulation_config:
        result = generate_simulation(simulation_generator_script, temp_simulation_config.name)
        if result.returncode != 0:
            return False
        print(f"Simulation generator {simulation_generator_script} generated config {temp_simulation_config.name}")
        return run_nons_in_temp_dir(nons_path, temp_simulation_config.name)
    
def check_two_generators(
        nons_path : str,
        topology_generator_script : str,
        ti_simulation_generator_script : str) -> bool:
    """
    Checks given topology and topology independent simulation generators:
    1) Generates topology using topology_generator_script
    2) Generates simulation using universal_simulation_generator
    3) Runs nons on given simulation
    Returns true if check succeed, false otherwise
    """
    with tempfile.NamedTemporaryFile(delete=True) as temp_topology_file:
        # Generate topology
        result = generate_topology(topology_generator_script, temp_topology_file.name)
        if result.returncode != 0:
            return False
        print(f"Topology generator {topology_generator_script} generated config {temp_topology_file.name}")

        with tempfile.NamedTemporaryFile(delete=True) as temp_simulation_file:
            # Generate simulation
            result = generate_simulation_by_topology(ti_simulation_generator_script, temp_topology_file.name, temp_simulation_file.name)
            if result.returncode != 0:
                return False
            print(f"Simulation generator {topology_generator_script} generated config {temp_simulation_file.name}"
                  f" for topology {temp_topology_file.name}")
            return run_nons_in_temp_dir(nons_path, temp_simulation_file.name).returncode == 0
        
def main():
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-e", "--executable", help="Path to the compiled project", required=True
    )
    parser.add_argument(
        "-g",
        "--generators",
        help="Path to the directory with generators",
        required=True,
    )
    args = parser.parse_args()

    nons_path = args.executable
    generators_path = args.generators
    
    topology_generators_scripts = get_generators_scripts(os.path.join(generators_path, "topology"))
    simulation_generators_scripts = get_generators_scripts(os.path.join(generators_path, "simulation"))
    ti_simulation_generators_scripts = get_generators_scripts(os.path.join(generators_path, "ti_simulation"))

    if len(ti_simulation_generators_scripts) == 0:
        print("Can not check topology generators: no topology independent simulation generators")
        sys.exit(-1)
    if len(topology_generators_scripts) == 0:
        print("Can not check topology impendent simulation generators: no topology generators")
        sys.exit(-1)

    # list of pairs(topology_generator, ti_simulation_generator)
    # that contains every topology generator and ti simulation generator once
    generators_pairs = [
        (topology_generator_script, ti_simulation_generators_scripts[0])
        for topology_generator_script in topology_generators_scripts
    ] + [
        (topology_generators_scripts[0], ti_simulation_generator_script)
        for ti_simulation_generator_script in ti_simulation_generators_scripts
    ]

    failed_pairs = list(filter(
        lambda generators : not check_two_generators(nons_path, generators[0], generators[1]),
        generators_pairs
    ))

    failed_simulation_generators = list(filter(
        lambda generator : not check_simulation_generator(nons_path, generator),
        simulation_generators_scripts
    ))

    if len(failed_pairs) != 0 or len(failed_simulation_generators) != 0:
        for simulation_generator in failed_simulation_generators:
            print(f"Failed check for simulation generator {simulation_generator}")
        for topology_generator, ti_simulation_generator in failed_pairs:
            print(f"Failed check for topology generator {topology_generator} and TI simulation generator {ti_simulation_generator}")
        sys.exit(-1)
    
    print("All generators succeed!")
        

if __name__ == "__main__":
    main()