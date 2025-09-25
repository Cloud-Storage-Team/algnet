from common import run_subprocess
import os
import argparse

SIMULATION_GENERATORS_DIR = os.path.dirname(__file__)

def parse_simulation_generator_args(config_path : str, help_description = ""):
    default_config_rel_path = os.path.relpath(config_path, os.getcwd())

    parser = argparse.ArgumentParser(description=help_description)

    parser.add_argument(
        "-c",
        "--config",
        help=f"Path to configuration file (default: {default_config_rel_path}). "\
              "See given default to get format & structure",
        default=default_config_rel_path
    )
    parser.add_argument("-o", "--output_path", help="Path to output topology config", required=True)
    
    return parser.parse_args()

def generate_simulation(simulation_generator_path : str, output_file : str, config_path : str | None = None):
    """
    Generates simulation config using given generator (simulation_generator_path)
    on given config (or use default if config_path is None).
    Puts generated simulation config to output_file.
    Returns result of generator run
    """
    args = [
        "python3",
        simulation_generator_path,
        "-o",
        output_file
    ]
    if config_path is not None:
        args += ["-c", config_path]
    return run_subprocess(args)