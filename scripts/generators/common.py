import yaml
import sys
import os
import argparse

# Reads yaml from file by given path
def load_yaml(path : str) -> dict:
    """Loads yaml conig from given file"""
    try:
        with open(path, 'r') as f:
            return yaml.safe_load(f)
    except FileNotFoundError:
        print(f"Error: Configuration file '{path}' not found.")
        sys.exit(1)
    except yaml.YAMLError as e:
        print(f"Error: Invalid YAML in configuration file: {e}")
        sys.exit(1)

def save_yaml(data, filename):
    """Save data as YAML to a file"""
    with open(filename, "w") as f:
        yaml.dump(data, f, sort_keys=False, default_flow_style=False)

def parse_args(config_path : str):
    default_config_rel_path = os.path.relpath(config_path, os.getcwd())

    parser = argparse.ArgumentParser(description="Generator of simulation config with all-to-all connections between hosts")

    parser.add_argument(
        "-c",
        "--config",
        help=f"Path to configuration file (default: {default_config_rel_path}). "\
              "See given default to get format & structure",
        default=default_config_rel_path
    )
    parser.add_argument("-o", "--output_path", help="Path to output topology config", required=True)
    
    return parser.parse_args()