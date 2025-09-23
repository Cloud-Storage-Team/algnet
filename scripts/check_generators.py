import argparse
import os
import tempfile
import subprocess
import sys
from collections.abc import Callable

from common import *

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

def find_generator_script(generator_dir : str) -> str:
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


def check_simulation_generator(nons_path : str, simulation_generator_script : str) -> bool:
    """"
    Checks given simulation generator:
    1) Generate simulation using simulation_generator_script
    2) Runs nons on it
    Returns true if check succeed, false otherwise
    """
    with tempfile.NamedTemporaryFile(delete=True) as temp_simulation_config:
        generator_args = [
            "python3",
            simulation_generator_script,
            "-o",
            temp_simulation_config.name
        ]
        result = run_subprocess(generator_args)
        if result.returncode != 0:
            return False
        print(f"Simulation generator {simulation_generator_script} generated config {temp_simulation_config.name}")
        return run_nons(nons_path, temp_simulation_config.name)
    
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
        topology_generator_args = [
            "python3",
            topology_generator_script,
            "-o",
            temp_topology_file.name
        ]
        result = run_subprocess(topology_generator_args)
        if result.returncode != 0:
            return False
        print(f"Topology generator {topology_generator_script} generated config {temp_topology_file.name}")

        with tempfile.NamedTemporaryFile(delete=True) as temp_simulation_file:
            # Generate simulation
            simulation_generator_args = [
                "python3",
                ti_simulation_generator_script,
                "-t",
                temp_topology_file.name,
                "-o",
                temp_simulation_file.name
            ]
            result = run_subprocess(simulation_generator_args)
            if result.returncode != 0:
                return False
            print(f"Simuation generator {topology_generator_script} generated config {temp_simulation_file.name}"
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
    
    topology_generators_scripts = get_generators_scripts(topology_generators_dir)
    simulation_generators_scripts = get_generators_scripts(simulation_generators_dir)
    ti_simulation_generators_scripts = get_generators_scripts(ti_simulation_generators_dir)

    # Checks topology generators using fiest topology independent generator
    if len(ti_simulation_generators_scripts) == 0:
        print(f"Can not check topology generators: not topology independent generators under {ti_simulation_generators_dir}")
        exit(-1)
    
    check_generators(
        topology_generators_scripts,
        lambda generator : check_two_generators(nons_path, generator, ti_simulation_generators_scripts[0]),
        "topology"
    )

    if len(ti_simulation_generators_scripts) > 1:
        if len(topology_generators_scripts) == 0:
            print(f"Can not check topology independent generators: not topology generators under {topology_generators_dir}")
            exit(-1)
        
        check_generators(
            ti_simulation_generators_scripts,
            lambda generator : check_two_generators(nons_path, topology_generators_scripts[0], generator),
            "topology independent simulation"
        )

    check_generators(
        simulation_generators_scripts,
        lambda generator : check_simulation_generator(nons_path, generator),
        "simulation"
    )

    exit(retcode)

if __name__ == "__main__":
    main()