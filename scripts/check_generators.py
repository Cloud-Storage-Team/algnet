import argparse
import os
import tempfile
import subprocess
import sys
from collections.abc import Callable

from common import *

def run_subprocess(subprocess_args : list[str], check_fails : bool = True):
    """
    Runs subprocess with given args
    Returns result of subprocess
    """
    result = subprocess.run(subprocess_args, capture_output=True, text=True)
    if result.returncode != 0 and check_fails:
        print(f"Command {subprocess_args} failed; stderr:", file=sys.stderr)
        print(result.stderr, file=sys.stderr)
    return result

def find_generator_script(generator_dir : str) -> str:
    """
    Searchs for generator (python script different from __init__.py) in generator_dir
    If there are suitable scripts or more that one, outs erro to stderr and retuns Nons
    Otherwise returns path to generator
    """
    python_generator_path = None
    for item in sorted(os.listdir(generator_dir)):
        item_path = os.path.join(generator_dir, item)
        if os.path.isdir(item_path):
            continue
        if not (item.endswith(".py") and item != "__init__.py"):
            continue

        if python_generator_path is None:
            print(f"Found python generator {item} in directory {generator_dir}")
            python_generator_path = item_path
        else:
            print(f"Found one more python script {item} in directory {generator_dir}; ignored")
    if (python_generator_path is None):
        print(f"Error: Can not find python script in {generator_dir}", file=sys.stderr)
    return python_generator_path

def get_topology_config_path(simulation_config_path : str):
    simulation_config = load_yaml(simulation_config_path)
    return simulation_config["topology_config_path"]

def run_nons(
        nons_path : str,
        simulation_config_path : str,
        ) -> bool:
    """
    Runs NoNs on with given config
    If simulation succeed, returns true
    Otherwice prints stderr of NoNS, simulation and topology configs
    """
    with tempfile.TemporaryDirectory() as temp_dir:
        nons_args = [
            nons_path,
            "-c",
            simulation_config_path,
            "--metrics-filter=", # run without metrics collection
            f"--output-dir={temp_dir}"
        ]
        result =  run_subprocess(nons_args, check_fails=False)
        if result.returncode != 0:
            print(f"Run NoNS failed!", file=sys.stderr)

            print(f"Simulation config (location: {simulation_config_path}) content:", file=sys.stderr)
            with open(simulation_config_path, "r") as f:
                print(f.read(), file=sys.stderr)

            # Prints topology config if it is not places in simulation config
            topology_config_path = get_topology_config_path(simulation_config_path)            
            if topology_config_path != simulation_config_path:
                print(f"Topology config (location: {topology_config_path}) content:", file=sys.stderr)
                with open(simulation_config_path, "r") as f:
                    print(f.read(), file=sys.stderr)

            print("=" * 30, file=sys.stderr)

            print("Nons stderr:", file=sys.stderr)
            print(result.stderr, file=sys.stderr)
            print("=" * 50, file=sys.stderr)

            return False
        print(f"Run NoNS on simulation config {simulation_config_path} succeed!")
    return True


def check_simulation_generator(nons_path : str, simulation_generator_dir_path : str) -> bool:
    """"
    Checks given simulation generator:
    1) Generate simulation using generator
    2) Runs nons on it
    Returns true if check succeed, false otherwise
    """
    
    python_generator_path = find_generator_script(simulation_generator_dir_path)
    if python_generator_path is None :
        return False
    
    with tempfile.NamedTemporaryFile(delete=True) as temp_simulation_config:
        generator_args = [
            "python3",
            python_generator_path,
            "-o",
            temp_simulation_config.name
        ]
        result = run_subprocess(generator_args)
        if result.returncode != 0:
            return False
        print(f"Simulation generator {python_generator_path} generated config {temp_simulation_config.name}")
        return run_nons(nons_path, temp_simulation_config.name)
    
def find_generator_dirs(generators_dir : str):
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
    
def check_topology_generator(
        nons_path : str,
        topology_generator_dir : str,
        topology_independent_simulation_generator : str):
    """
    Checks given topology generator:
    1) Generates topology using script from topology_generator_dir
    2) Generates simulation using universal_simulation_generator
    3) Runs nons on given simulation
    Returns true if check succeed, false otherwise
    """
    topology_generator = find_generator_script(topology_generator_dir)
    if topology_generator is None:
        return False
    with tempfile.NamedTemporaryFile(delete=True) as temp_topology_file:
        # Generate topology
        topology_generator_args = [
            "python3",
            topology_generator,
            "-o",
            temp_topology_file.name
        ]
        result = run_subprocess(topology_generator_args)
        if result.returncode != 0:
            return False
        print(f"Topology generator {topology_generator} generated config {temp_topology_file.name}")

        with tempfile.NamedTemporaryFile(delete=True) as temp_simulation_file:
            # Generate simulation
            simulation_generator_args = [
                "python3",
                topology_independent_simulation_generator,
                "-t",
                temp_topology_file.name,
                "-o",
                temp_simulation_file.name
            ]
            result = run_subprocess(simulation_generator_args)
            if result.returncode != 0:
                return False
            print(f"Simuation generator {topology_generator} generated config {temp_simulation_file.name}"
                  f" for topology {temp_topology_file.name}")
            return run_nons(nons_path, temp_simulation_file.name)

def get_failed_generators(generators : list[str], generator_checker : Callable[[str], bool]) -> list[str]:
    """
    Runs generator_checker on each item from generators
    Returns list of generators that failes generator_checker
    """
    return list(
        filter(
            lambda generator : not generator_checker(generator),
            generators
        )
    )
        
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

    retcode = 0

    def check_generators(generators : list[str], generator_checker : Callable[[str], bool], generator_type : str):
        nonlocal retcode
        failed_generators = get_failed_generators(
            generators,
            generator_checker
        )

        if len(failed_generators) != 0:
            print(f"List of failed {generator_type} generators: ", file=sys.stderr)
            print(*failed_generators)
            retcode = -1
        else:
            print(f"All {generator_type} generators succeed!")
        print()
    
    topology_generators_dir = os.path.join(generators_path, "topology")
    ti_simulation_generators_dir = os.path.join(generators_path, "ti_simulation")
    simulation_generators_dir = os.path.join(generators_path, "simulation")
    
    topology_generators_dir = find_generator_dirs(topology_generators_dir)
    simulation_generators_dirs = find_generator_dirs(simulation_generators_dir)
    ti_simulation_generators_dirs = find_generator_dirs(ti_simulation_generators_dir)

    # Checks topology generators using fiest topology independent generator
    if len(ti_simulation_generators_dirs) == 0:
        print(f"Can not check topology generators: not topology independent generators under {ti_simulation_generators_dir}")
        exit(-1)
    
    ti_simulation_generator = find_generator_script(ti_simulation_generators_dirs[0])
    if ti_simulation_generator is None:
        print("Can not find script in directory of first topology independent generator", file=sys.stderr)
    check_generators(
        topology_generators_dir,
        lambda generator : check_topology_generator(nons_path, generator, ti_simulation_generator),
        "topology"
    )

    check_generators(
        simulation_generators_dirs,
        lambda generator : check_simulation_generator(nons_path, generator),
        "simulation"
    )

    exit(retcode)

if __name__ == "__main__":
    main()