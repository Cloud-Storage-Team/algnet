import os
import argparse

def parse_args(config_name = "default_config.yml"):
    curr_file_path = os.path.abspath(__file__)
    curr_file_dir_path = os.path.dirname(curr_file_path)
    default_config_full_path = os.path.join(curr_file_dir_path, config_name)
    default_config_rel_path = os.path.relpath(default_config_full_path, os.getcwd())

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