import yaml
import argparse
import sys
import os

def save_yaml(data, filename):
    """Save data as YAML to a file"""
    with open(filename, "w") as f:
        yaml.dump(data, f, sort_keys=False, default_flow_style=False)

def load_config(config_file):
    try:
        with open(config_file, 'r') as f:
            return yaml.safe_load(f)
    except FileNotFoundError:
        print(f"Error: Configuration file '{config_file}' not found.")
        sys.exit(1)
    except yaml.YAMLError as e:
        print(f"Error: Invalid YAML in configuration file: {e}")
        sys.exit(1)

def add_bidirectional_link(config, from_node, to_node, link_counter, preset = "default"):
    config["links"][f"link{link_counter}"] = {
        "from": from_node, "to": to_node, "preset-name": preset
    }
    link_counter += 1
    config["links"][f"link{link_counter}"] = {
        "from": to_node, "to": from_node, "preset-name": preset
    }
    return link_counter + 1

def generate_fat_tree_config(config_params):
    switch_ports_count = config_params["switch_ports_count"]
    link_presets = config_params["link_presets"]
    switch_presets = config_params["switch_presets"]
    packet_spraying = config_params["packet_spraying"]
    
    if switch_ports_count % 2 != 0 or switch_ports_count < 2:
        raise ValueError(f"Switch's number of ports must be an even integer >= 2, but got {switch_ports_count}")
    
    num_pods = switch_ports_count
    edge_per_pod = switch_ports_count // 2
    aggr_per_pod = switch_ports_count // 2
    hosts_per_pod = edge_per_pod * (switch_ports_count // 2)
    core_switches = (switch_ports_count // 2) ** 2
    total_hosts = (switch_ports_count ** 3) // 4
    hosts_per_edge = switch_ports_count // 2
    core_per_aggr = switch_ports_count // 2
    
    config = {
        "presets": {
            "link": link_presets,
            "switch": switch_presets
        },
        "packet-spraying": packet_spraying,
        "hosts": {},
        "switches": {},
        "links": {}
    }

    host_name = lambda pod_idx, host_idx: f"pod{pod_idx}_host{host_idx}"
    aggr_name = lambda pod_idx, aggr_idx: f"pod{pod_idx}_aggr{aggr_idx}"
    edge_name = lambda pod_idx, edge_idx: f"pod{pod_idx}_edge{edge_idx}"
    core_name = lambda core_idx: f"core{core_idx}"
    
    for p in range(1, num_pods + 1):
        for h in range(1, hosts_per_pod + 1):
            config["hosts"][host_name(p, h)] = {"layer": 3}
    
    for i in range(1, core_switches + 1):
        config["switches"][f"core{i}"] = {"preset-name": "core", "layer": 0}
    
    for p in range(1, num_pods + 1):
        for a in range(1, aggr_per_pod + 1):
            config["switches"][aggr_name(p, a)] = {"preset-name": "aggr", "layer": 1}
        for e in range(1, edge_per_pod + 1):
            config["switches"][edge_name(p, e)] = {"preset-name": "edge", "layer": 2}

    link_counter = 1
    
    # Edge-host
    for pod_idx in range(1, num_pods + 1):
        for edge_idx in range(1, edge_per_pod + 1):
            for h in range(1, hosts_per_edge + 1):
                host_idx = (edge_idx - 1) * hosts_per_edge + h
                link_counter = add_bidirectional_link(
                    config,
                    host_name(pod_idx, host_idx),
                    edge_name(pod_idx, edge_idx),
                    link_counter,
                    "edge-host"
                )
    
    # Aggr-edge
    for pod_idx in range(1, num_pods + 1):
        for edge_idx in range(1, edge_per_pod + 1):
            for aggr_idx in range(1, aggr_per_pod + 1):
                link_counter = add_bidirectional_link(
                    config,
                    edge_name(pod_idx, edge_idx),
                    aggr_name(pod_idx, aggr_idx),
                    link_counter,
                    "aggr-edge"
                )
    
    # Aggr-core
    for pod_idx in range(1, num_pods + 1):
        for aggr_idx in range(1, aggr_per_pod + 1):
            core_offset = (aggr_idx - 1) * core_per_aggr
            for core_idx in range(1, core_per_aggr + 1):
                core_id = core_offset + core_idx
                link_counter = add_bidirectional_link(
                    config,
                    aggr_name(pod_idx, aggr_idx),
                    core_name(core_id),
                    link_counter,
                    "aggr-core"
                )
    
    return config

def write_config_to_file(config, output_path):   
    with open(output_path, 'w') as f:
        yaml.dump(config, f, sort_keys=False, width=120, indent=2)
    
    print(f"Configuration written to {output_path}")
    return output_path

if __name__ == "__main__":
    # Set up command-line argument parsing
    parser = argparse.ArgumentParser(description='Generate Fat-Tree network configuration. You may see more about it here https://packetpushers.net/blog/demystifying-dcn-topologies-clos-fat-trees-part2/')
    curr_file_path = os.path.realpath(__file__)
    curr_dir_path = os.path.dirname(curr_file_path)
    default_config_full_path = os.path.join(curr_dir_path, "fat_tree_config.yaml")
    default_config_abs_path = os.path.relpath(default_config_full_path, os.getcwd())

    parser.add_argument('-c', '--config', 
                        default=default_config_abs_path,
                        help=f'Path to configuration file (default: {default_config_abs_path}). See given default to get format & structure of this config')
    
    parser.add_argument('-o', '--output_path',
        default='fat_tree_topology.yaml',
        help='Path to the output topology config file',
    )

    
    args = parser.parse_args()
    
    # Load configuration from file
    config_params = load_config(args.config)
    
    # Generate and write the fat-tree configuration
    topology = generate_fat_tree_config(config_params)
    save_yaml(topology, args.output_path)