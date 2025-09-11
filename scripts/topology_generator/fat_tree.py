import yaml

def add_bidirectional_link(config, from_node, to_node, link_counter, preset = "default"):
    config["links"][f"link{link_counter}"] = {
        "from": from_node, "to": to_node, "preset-name": preset
    }
    link_counter += 1
    config["links"][f"link{link_counter}"] = {
        "from": to_node, "to": from_node, "preset-name": preset
    }
    return link_counter + 1

def generate_fat_tree_config(switch_ports_count):
    if k % 2 != 0 or k < 2:
        raise ValueError(f"Switch's number of ports must be an even integer >= 2, but got {switch_ports_count}")
    
    num_pods = k
    edge_per_pod = k // 2
    aggr_per_pod = k // 2
    hosts_per_pod = edge_per_pod * (k // 2)
    core_switches = (k // 2) ** 2
    total_hosts = (k ** 3) // 4
    senders = total_hosts // 2
    receivers = total_hosts // 2
    
    config = {
        "presets": {
            "link": {
                "default": {
                    "latency": "1ns",
                    "throughput": "1Gbps",
                    "ingress_buffer_size": "4096B",
                    "egress_buffer_size": "4096B"
                },
                "medium_speed": {
                    "latency": "1ns",
                    "throughput": "10Gbps",
                    "ingress_buffer_size": "4096B",
                    "egress_buffer_size": "4096B"
                },
                "high_speed": {
                    "latency": "1ns",
                    "throughput": "100Gbps",
                    "ingress_buffer_size": "4096B",
                    "egress_buffer_size": "4096B"
                }
            },
            "switch": {
                "edge-preset": {
                    "ecn": {"min": 0.2, "max": 0.3, "probability": 0.5}
                },
                "aggr-preset": {
                    "ecn": {"min": 0.2, "max": 0.3, "probability": 0.7}
                },
                "core-preset": {
                    "ecn": {"min": 0.2, "max": 0.3, "probability": 1.0}
                }
            }
        },
        "packet-spraying": {"type": "ecmp"},
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
        config["switches"][f"core{i}"] = {"preset-name": "core-preset", "layer": 0}
    
    for p in range(1, num_pods + 1):
        for a in range(1, aggr_per_pod + 1):
            config["switches"][aggr_name(p, a)] = {"preset-name": "aggr-preset", "layer": 1}
        for e in range(1, edge_per_pod + 1):
            config["switches"][edge_name(p, e)] = {"preset-name": "edge-preset", "layer": 2}

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
                    link_counter
                )
    
    # Aggr-edge
    for pod in range(num_pods):
        for edge_idx in range(1, edge_per_pod + 1):
            edge_id = pod * edge_per_pod + edge_idx
            for aggr_idx in range(1, aggr_per_pod + 1):
                aggr_id = pod * aggr_per_pod + aggr_idx
                link_counter = add_bidirectional_link(config, f"edge{edge_id}", f"aggr{aggr_id}", link_counter, "medium_speed")
    
    # Aggr-core
    for pod in range(num_pods):
        for aggr_idx in range(1, aggr_per_pod + 1):
            aggr_id = pod * aggr_per_pod + aggr_idx
            core_offset = (aggr_idx - 1) * (k // 2)
            for core_idx in range(1, (k // 2) + 1):
                core_id = core_offset + core_idx
                link_counter = add_bidirectional_link(config, f"aggr{aggr_id}", f"core{core_id}", link_counter, "high_speed")
    
    return config

def write_config_to_file(k):
    config = generate_fat_tree_config(k)
    filename = f"fat_tree_k{k}.yaml"
    
    with open(filename, 'w') as f:
        yaml.dump(config, f, sort_keys=False, width=120, indent=2)
    
    print(f"Configuration written to {filename}")
    return filename

if __name__ == "__main__":
    k_value = 8
    output_file = write_config_to_file(k_value)