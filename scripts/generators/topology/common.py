
import os
import argparse
from common import run_subprocess


TOPOLOGY_GENERATORS_DIR = os.path.dirname(__file__)

def parse_topology_generator_args(config_path : str, help_description = ""):
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

def generate_topology(topology_generator_path : str, output_file : str, config_path : str | None = None):
    """
    Generates topology config using given generator (topology_generator_path)
    on given config (or use default if config_path is None).
    Puts generated topology config to output_file.
    Returns result of generator run
    """
    args = [
        "python3",
        topology_generator_path,
        "-o",
        output_file
    ]
    if config_path is not None:
        args += ["-c", config_path]
    return run_subprocess(args)

class LinkGenerator:
    def __init__(self, topology : dict):
        self.next_link_num = 0
        self.topology = topology

    def add_directed_link(self, source_id : str, dest_id : str, preset_name : str = "default") -> int:
        """
        Adds to topology link from source_id to dest_id
        Link id is link_<next_link_num>
        """
        link_name = f"link_{self.next_link_num}"

        if "links" not in self.topology:
            self.topology["links"] = {}    
        
        self.topology["links"][link_name] = {
            "from" : source_id,
            "to" : dest_id
        }
        self.topology["links"][link_name]["preset-name"] = preset_name
        self.next_link_num += 1

    def add_bidirectional_link(self,
                               first_device_id : str,
                               second_device_id : str,
                               forward_preset_name : str = "default",
                               backwards_preset_name : str = "default"):
        """
        Adds links two links between first_device_id and second_device_id:
        for link first->second used forward_preset_name
        for link second->first used backward_preset_name
        """
        self.add_directed_link(first_device_id, second_device_id, forward_preset_name)
        self.add_directed_link(second_device_id, first_device_id, backwards_preset_name)
        
    def add_bidirectional_link_same_preset(self,
                               first_device_id : str,
                               second_device_id : str,
                               preset_name : str):
        self.add_bidirectional_link(first_device_id, second_device_id, preset_name, preset_name)