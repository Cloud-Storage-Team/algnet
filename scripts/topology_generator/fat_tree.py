import yaml

def generate_fat_tree_config(k):
    if k % 2 != 0 or k < 2:
        raise ValueError("k must be an even integer >= 2")
    
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
    
    for i in range(1, senders + 1):
        config["hosts"][f"sender{i}"] = {"layer": 3}
    for i in range(1, receivers + 1):
        config["hosts"][f"receiver{i}"] = {"layer": 3}
    
    for i in range(1, core_switches + 1):
        config["switches"][f"core{i}"] = {"preset-name": "core-preset", "layer": 0}
    
    for p in range(num_pods):
        for a in range(1, aggr_per_pod + 1):
            aggr_id = p * aggr_per_pod + a
            config["switches"][f"aggr{aggr_id}"] = {"preset-name": "aggr-preset", "layer": 1}
        for e in range(1, edge_per_pod + 1):
            edge_id = p * edge_per_pod + e
            config["switches"][f"edge{edge_id}"] = {"preset-name": "edge-preset", "layer": 2}

    link_counter = 1
    
    # Edge-host
    for pod in range(num_pods):
        for edge_idx in range(1, edge_per_pod + 1):
            edge_id = pod * edge_per_pod + edge_idx
            for h in range(1, k // 2 + 1):
                host_num = pod * hosts_per_pod + (edge_idx - 1) * (k // 2) + h
                host_type = "sender" if host_num <= total_hosts // 2 else "receiver"
                actual_host_num = host_num if host_type == "sender" else host_num - senders
                
                config["links"][f"link{link_counter}"] = {
                    "from": f"{host_type}{actual_host_num}",
                    "to": f"edge{edge_id}",
                    "preset-name": "default"
                }
                link_counter += 1
                
                config["links"][f"link{link_counter}"] = {
                    "from": f"edge{edge_id}",
                    "to": f"{host_type}{actual_host_num}",
                    "preset-name": "default"
                }
                link_counter += 1
    
    # Aggr-edge
    for pod in range(num_pods):
        for edge_idx in range(1, edge_per_pod + 1):
            edge_id = pod * edge_per_pod + edge_idx
            for aggr_idx in range(1, aggr_per_pod + 1):
                aggr_id = pod * aggr_per_pod + aggr_idx
                
                config["links"][f"link{link_counter}"] = {
                    "from": f"edge{edge_id}",
                    "to": f"aggr{aggr_id}",
                    "preset-name": "default"
                }
                link_counter += 1
                
                config["links"][f"link{link_counter}"] = {
                    "from": f"aggr{aggr_id}",
                    "to": f"edge{edge_id}",
                    "preset-name": "default"
                }
                link_counter += 1
    
    # Aggr-core
    for pod in range(num_pods):
        for aggr_idx in range(1, aggr_per_pod + 1):
            aggr_id = pod * aggr_per_pod + aggr_idx
            core_offset = (aggr_idx - 1) * (k // 2)
            for core_idx in range(1, (k // 2) + 1):
                core_id = core_offset + core_idx
                
                config["links"][f"link{link_counter}"] = {
                    "from": f"aggr{aggr_id}",
                    "to": f"core{core_id}",
                    "preset-name": "high_speed"
                }
                link_counter += 1
                
                config["links"][f"link{link_counter}"] = {
                    "from": f"core{core_id}",
                    "to": f"aggr{aggr_id}",
                    "preset-name": "high_speed"
                }
                link_counter += 1
    
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