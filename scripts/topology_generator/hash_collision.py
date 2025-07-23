import yaml
import argparse
import sys


def generate_topology(
    number_switch_per_layer,
    link_latency,
    link_throughput,
    ingress_buffer_size,
    egress_buffer_size,
):
    sender_name = "sender"
    receiver_name = "receiver"

    topology = {
        "devices": {
            sender_name : {"type" : "host"},
            receiver_name : {"type" : "host"}
        },
        "links": {}
    }
    
    base_index = 1
    def add_link(src_device : str, dest_device : str):
        nonlocal base_index
        name = f"link_{base_index}"
        base_index += 1
        
        topology["links"][name] = {
            "from": src_device,
            "to" : dest_device,
            "latency" : f"{link_latency}ns",
            "throughput" : f"{link_throughput}Gbps",
            "ingress_buffer_size" : f"{ingress_buffer_size}B",
            "egress_buffer_size" : f"{egress_buffer_size}B"
        }

    switch_nums_range = range(1, number_switch_per_layer + 1)

    switch_first_layer_names = [f"switch_{i}_0" for i in switch_nums_range]
    switch_second_layer_names = [[f"switch_{i}_{j}" for j in switch_nums_range] for i in switch_nums_range] 
    # Add first layer
    for switch_name in switch_first_layer_names:
        topology["devices"][switch_name] = {"type" : "switch"}

        add_link(sender_name, switch_name)
        add_link(switch_name, sender_name)

    # Add second layer
    for i, first_layer_switch_name in enumerate(switch_first_layer_names):
        for second_layer_switch_name in switch_second_layer_names[i]:
            topology["devices"][second_layer_switch_name] = {"type" : "switch"}

            add_link(first_layer_switch_name, second_layer_switch_name)
            add_link(second_layer_switch_name, first_layer_switch_name)

            add_link(second_layer_switch_name, receiver_name)
            add_link(receiver_name, second_layer_switch_name)


    return topology


def save_yaml(data, filename):
    """Save data as YAML to a file"""
    with open(filename, "w") as f:
        yaml.dump(data, f, sort_keys=False, default_flow_style=False)


def parse_arguments():
    """Parse and validate command line arguments"""
    parser = argparse.ArgumentParser(
        description="Generates hash collision topology YAML file."
    )
    parser.add_argument(
        "--num_switch_per_layer",
        "-n",
        type=int,
        default=1,
        help="Number of switch on each of two layers",
    )
    parser.add_argument(
        "--throughput",
        "-t",
        type=int,
        default=10,
        help="Throughput of each link, Gbit per s",
    )
    parser.add_argument(
        "--latency",
        "-l",
        type=int,
        default=0,
        help="Latency of each link, ns",
    )
    parser.add_argument(
        "--egress_buffers_size",
        "-ebs",
        type=int,
        default=4096,
        help="Size of egress buffers, bytes",
    )
    parser.add_argument(
        "--ingress_buffers_size",
        "-ibs",
        type=int,
        default=4096,
        help="Size of ingress buffers, bytes",
    )
    parser.add_argument(
        "--output_path", "-o",
        default="hash_collision_topology.yml",
        help="Path to the output topology config file",
    )

    args = parser.parse_args()

    # Validate inputs
    if args.num_switch_per_layer < 1:
        print("Error: Number of switches must be at least 1", file=sys.stderr)
        sys.exit(1)
    return args


def main():
    # Parse command line arguments
    args = parse_arguments()

    # Generate topology
    topology = generate_topology(
        args.num_switch_per_layer,
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
