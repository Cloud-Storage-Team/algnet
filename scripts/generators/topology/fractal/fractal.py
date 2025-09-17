import yaml
import argparse
import sys
import os

from parse_args import parse_args

def generate_topology(
    number_switch_per_layer,
    depth,
    link_latency,
    link_throughput,
    ingress_buffer_size,
    egress_buffer_size,
):
    sender_name = "sender"
    receiver_name = "receiver"

    topology = {
        "presets" : {
            "link" : {"default" : {
                "latency" : f"{link_latency}ns",
                "throughput" : f"{link_throughput}Gbps",
                "ingress_buffer_size" : f"{ingress_buffer_size}B",
                "egress_buffer_size" : f"{egress_buffer_size}B"
            }}
        },
        "packet-spraying" : {"type" : "ecmp"},
        "hosts": {
            sender_name : {},
            receiver_name : {}
        },
        "switches":{},
        "links": {}
    }

    base_index = 1
    def add_link(src_device : str, dest_device : str):
        nonlocal base_index
        name = f"link-{base_index}"
        base_index += 1

        topology["links"][name] = {
            "from": src_device,
            "to" : dest_device
        }

    switch_nums_range = range(1, number_switch_per_layer + 1)

    prev_layer_names = [sender_name]
    current_layer_names = [[f"switch-{i}" for i in switch_nums_range]]
    # Add layers
    for _ in range (depth):
        for i, prev_layer_name in enumerate(prev_layer_names):
            for switch_name in current_layer_names[i]:
                topology["switches"][switch_name] = {"type" : "switch"}

                add_link(prev_layer_name, switch_name)
                add_link(switch_name, prev_layer_name)

        prev_layer_names = [name for names in current_layer_names for name in names]
        current_layer_names = [[f"{name}-{i}" for i in switch_nums_range] for name in prev_layer_names]

    # Add links betwwen last layer and receiver
    for name in prev_layer_names:
        add_link(name, receiver_name)
        add_link(receiver_name, name)


    return topology


def save_yaml(data, filename):
    """Save data as YAML to a file"""
    with open(filename, "w") as f:
        yaml.dump(data, f, sort_keys=False, default_flow_style=False)


def main():
    # Parse command line arguments
    
    args = parse_args(os.path.join(os.path.dirname(__file__), "default_config.yml"))

    print(args.config)

    # Generate topology
    topology = generate_topology(
        args.num_childs,
        args.depth,
        args.latency,
        args.throughput,
        args.ingress_buffers_size,
        args.egress_buffers_size
    )

    save_yaml(topology, args.output_path)
    print(
        f"Topology file saved as {args.output_path}"
    )


if __name__ == "__main__":
    main()
