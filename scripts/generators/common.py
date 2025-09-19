import os
import argparse

def parse_generator_args(config_path : str, help_description = ""):
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