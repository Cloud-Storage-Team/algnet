import argparse
import os
import sys
import yaml

# To import common
curr_file_path = os.path.abspath(__file__)
curr_file_dir_path = os.path.dirname(curr_file_path)
common_dir_path = os.path.dirname(curr_file_dir_path)

if common_dir_path not in sys.path:
    sys.path.append(common_dir_path)

from common import load_config, save_yaml

def parse_args():
    default_config_full_path = os.path.join(curr_file_dir_path, "all-to-all_config.yml")
    default_config_rel_path = os.path.relpath(default_config_full_path, os.getcwd())

    parser = argparse.ArgumentParser(description="Generator of simulation config with all-to-all connections between hosts")

    parser.add_argument(
        "-c",
        "--config",
        help=f"Path to configuration file (default: {default_config_rel_path}). "\
              "See given default to get format & structure",
        default=default_config_rel_path
    )
    parser.add_argument("-o", "--output_path", help="Path to output simulation config", required=True)
    
    return parser.parse_args()

def get_host_names(topology : dict) -> list[str]:
    try:
        return list(topology["hosts"].keys())
    except KeyError as e:
        raise RuntimeError(f"Topology config missing field {e}")

def get_topology_path(config : dict, config_dir : str) -> str:
    try:
        topology_path_from_config = config["topology_config_path"]
        return os.path.join(config_dir, topology_path_from_config)
    except KeyError as e:
        raise RuntimeError(f"Config missing field {e}")
    
def generate_simulation_config(
        host_names : list[str],
        config : dict,
        topology_config_path : str) -> dict:
    
    try:
        flows_per_connection = config["flows_per_connection"]
        presets = config["presets"]
    except KeyError as e:
        raise RuntimeError(f"Config missing field {e}")

    simulation_config = {
        "topology_config_path" : topology_config_path,
        "presets" : presets,
        "connections" : {}
    }

    for sender_id in host_names:
        for receiver_id in host_names:
            if sender_id == receiver_id:
                continue
            connection_name = f"conn_from_{sender_id}_to_{receiver_id}"

            connection = {
                "sender_id": sender_id,
                "receiver_id": receiver_id,
                "flows" : {}
            }

            for flow_num in range(1, flows_per_connection + 1):
                connection["flows"][f"flow_{flow_num}"] = {}
            
            simulation_config["connections"][connection_name] = connection 
    
    return simulation_config
def main():
    
    args = parse_args()

    config_path = args.config
    config_dir = os.path.dirname(config_path)
    output_path = args.output_path

    config = load_config(config_path)
    topology_path = get_topology_path(config, config_dir)
    topology = load_config(topology_path)
    host_names = get_host_names(topology)

    output_dir = os.path.dirname(output_path)
    topology_path_rel_from_output = os.path.relpath(topology_path, output_dir)
    
    simulation_config = generate_simulation_config(host_names, config, topology_path_rel_from_output)

    save_yaml(simulation_config, output_path)
    print(f"Your simulation config saved to {output_path}")
    

if __name__ == "__main__":
    main()