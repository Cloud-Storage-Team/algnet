import os

from common import *
from generators.topology.common import *

def generate_fat_tree_config(config_params):
    try:
        presets = config_params["presets"]
        switch_ports_count = config_params["switch_ports_count"]
        packet_spraying = config_params["packet_spraying"]
    except KeyError as e:
        raise KeyError(f"Config missing value {e}")
        
    
    if switch_ports_count % 2 != 0 or switch_ports_count < 2:
        raise ValueError(f"Switch's number of ports must be an even integer >= 2, but got {switch_ports_count}")
    
    num_pods = switch_ports_count
    edge_per_pod = switch_ports_count // 2
    aggr_per_pod = switch_ports_count // 2
    hosts_per_pod = edge_per_pod * (switch_ports_count // 2)
    core_switches = (switch_ports_count // 2) ** 2
    hosts_per_edge = switch_ports_count // 2
    core_per_aggr = switch_ports_count // 2
    
    config = {
        "presets": presets,
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
        config["switches"][core_name(i)] = {"preset-name": "core", "layer": 0}
    
    for p in range(1, num_pods + 1):
        for a in range(1, aggr_per_pod + 1):
            config["switches"][aggr_name(p, a)] = {"preset-name": "aggr", "layer": 1}
        for e in range(1, edge_per_pod + 1):
            config["switches"][edge_name(p, e)] = {"preset-name": "edge", "layer": 2}

    link_generator = LinkGenerator(config)
    # Edge-host
    for pod_idx in range(1, num_pods + 1):
        for edge_idx in range(1, edge_per_pod + 1):
            for h in range(1, hosts_per_edge + 1):
                host_idx = (edge_idx - 1) * hosts_per_edge + h
                link_generator.add_bidirectional_link_same_preset(
                    host_name(pod_idx, host_idx),
                    edge_name(pod_idx, edge_idx),
                    "edge-host"
                )
    
    # Aggr-edge
    for pod_idx in range(1, num_pods + 1):
        for edge_idx in range(1, edge_per_pod + 1):
            for aggr_idx in range(1, aggr_per_pod + 1):
                link_generator.add_bidirectional_link_same_preset(
                    edge_name(pod_idx, edge_idx),
                    aggr_name(pod_idx, aggr_idx),
                    "aggr-edge"
                )
    
    # Aggr-core
    for pod_idx in range(1, num_pods + 1):
        for aggr_idx in range(1, aggr_per_pod + 1):
            core_offset = (aggr_idx - 1) * core_per_aggr
            for core_idx in range(1, core_per_aggr + 1):
                core_id = core_offset + core_idx
                link_generator.add_bidirectional_link_same_preset(
                    aggr_name(pod_idx, aggr_idx),
                    core_name(core_id),
                    "aggr-core",
                )
    
    return config

if __name__ == "__main__":
    args = parse_topology_generator_args(
        os.path.join(os.path.dirname(__file__), "default_config.yml"),
        'Generate Fat-Tree network configuration.'\
        'You may see more about it here: '\
        'https://packetpushers.net/blog/demystifying-dcn-topologies-clos-fat-trees-part2/'
    )
    
    # Load configuration from file
    config_params = load_yaml(args.config)
    
    # Generate and write the fat-tree configuration
    topology = generate_fat_tree_config(config_params)
    save_yaml(topology, args.output_path)
    print(f"Fat-tree topology saved to {args.output_path}")