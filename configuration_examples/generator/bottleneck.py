import yaml
import argparse
import sys
import os


def generate_topology(
    num_senders,
    num_receivers,
    num_switches,
    switch_name="switch",
    link_latency="0ns",
    link_throughput="100Gbps",
    ingress_buffer_size="1024000B",
    egress_buffer_size="1024000B",
):
    topology = {"devices": {}, "links": {}}

    sender_switch_name = "switch0"
    receiver_switch_name = f"switch{num_switches - 1}"

    # Add senders
    for i in range(0, num_senders):
        sender_name = f"sender{i}"
        topology["devices"][sender_name] = {"type": "host"}
        base_index = 2 * i

        # Add link from sender to switch
        link_name = f"link{base_index}"
        topology["links"][link_name] = {
            "from": sender_name,
            "to": sender_switch_name,
            "latency": link_latency,
            "throughput": link_throughput,
            "ingress_buffer_size": ingress_buffer_size,
            "egress_buffer_size": egress_buffer_size,
        }

        # Add link from switch to sender
        link_name = f"link{base_index + 1}"
        topology["links"][link_name] = {
            "from": sender_switch_name,
            "to": sender_name,
            "latency": link_latency,
            "throughput": link_throughput,
            "ingress_buffer_size": ingress_buffer_size,
            "egress_buffer_size": egress_buffer_size,
        }

    # Add receivers
    for i in range(0, num_receivers):
        receiver_name = f"receiver{i}"
        topology["devices"][receiver_name] = {"type": "host"}
        base_index = 2 * num_senders + 2 * i

        # Add link from switch to receiver
        link_name = f"link{base_index}"
        topology["links"][link_name] = {
            "from": receiver_switch_name,
            "to": receiver_name,
            "latency": link_latency,
            "throughput": link_throughput,
            "ingress_buffer_size": ingress_buffer_size,
            "egress_buffer_size": egress_buffer_size,
        }

        # Add link from receiver to switch
        link_name = f"link{base_index + 1}"
        topology["links"][link_name] = {
            "from": receiver_name,
            "to": receiver_switch_name,
            "latency": link_latency,
            "throughput": link_throughput,
            "ingress_buffer_size": ingress_buffer_size,
            "egress_buffer_size": egress_buffer_size,
        }

    # Add the switches
    for i in range(0, num_switches):
        topology["devices"][switch_name] = {"type": f"switch{i}", "threshold": 0.7}

    # Add links between switches
    for i in range(0, num_switches - 1):
        # Forward link
        link_name = f"switch_link_{i}_to_{i + 1}"
        topology["links"][link_name] = {
            "from": f"switch{i}",
            "to": f"switch{i + 1}",
            "latency": link_latency,
            "throughput": link_throughput,
            "ingress_buffer_size": ingress_buffer_size,
            "egress_buffer_size": egress_buffer_size,
        }

        # Backward link
        link_name = f"switch_link_{i + 1}_to_{i}"
        topology["links"][link_name] = {
            "from": f"switch{i + 1}",
            "to": f"switch{i}",
            "latency": link_latency,
            "throughput": link_throughput,
            "ingress_buffer_size": ingress_buffer_size,
            "egress_buffer_size": egress_buffer_size,
        }

    return topology


def generate_simulation(
    topology_file,
    num_senders,
    num_receivers,
    flows,
    simulation_time,
    packet_interval,
    number_of_packets,
    packet_size=1500,
    algorithm="tcp",
):
    """
    Generate a simulation YAML structure with flows between senders and receivers.
    """
    simulation = {
        "topology_config_path": topology_file,
        "flows": {},
        "algorithm": algorithm,
        "simulation_time": simulation_time,
    }

    if flows == "1-to-1":

        # One sender to one receiver
        # If there are more senders than receivers, extra senders will connect to last receiver
        # If there are more receivers than senders, extra receivers won't have flows
        flow_id = 0
        for i in range(0, num_senders):
            flow_name = f"flow{flow_id}"
            flow_id += 1
            simulation["flows"][flow_name] = {
                "sender_id": f"sender{i}",
                "receiver_id": f"receiver{min(i,num_receivers-1)}",
                "packet_size": packet_size,
                "packet_interval": packet_interval,
                "number_of_packets": number_of_packets,
            }
        return simulation

    elif flows == "all-to-all":
        # Each sender to all receivers
        flow_id = 0
        for i in range(0, num_senders):
            for j in range(0, num_receivers):
                flow_name = f"flow{flow_id}"
                flow_id += 1
                simulation["flows"][flow_name] = {
                    "sender_id": f"sender{i}",
                    "receiver_id": f"receiver{j}",
                    "packet_size": packet_size,
                    "packet_interval": packet_interval,
                    "number_of_packets": number_of_packets,
                }
        return simulation

    else:
        print("Error: Unknown flows option value")


def save_yaml(data, filename):
    """Save data as YAML to a file"""
    with open(filename, "w") as f:
        yaml.dump(data, f, sort_keys=False, default_flow_style=False)


def parse_arguments():
    """Parse and validate command line arguments"""
    parser = argparse.ArgumentParser(
        description="Generate topology and simulation YAML files."
    )
    parser.add_argument(
        "--senders", type=int, required=True, help="Number of sender devices"
    )
    parser.add_argument(
        "--receivers", type=int, required=True, help="Number of receiver devices"
    )
    parser.add_argument(
        "--switches",
        type=int,
        default=1,
        help="Number of switches between senders and receivers",
    )
    parser.add_argument(
        "--topology",
        default="bottleneck_topology.yml",
        help="Output filename for topology file",
    )
    parser.add_argument(
        "--simulation",
        default="bottleneck_simulation.yml",
        help="Output filename for simulation file",
    )
    parser.add_argument(
        "--topology-dir",
        default="../topology_examples/",
        help="Path to the topology config file",
    )
    parser.add_argument(
        "--simulation-dir",
        default="../simulation_examples/",
        help="Path to the simulation config file",
    )
    parser.add_argument(
        "--simulation-time", type=int, default=50000, help="Time of the simulation, ns"
    )
    parser.add_argument(
        "--packets",
        type=int,
        default=100,
        help="Number of packets sending by each sender",
    )
    parser.add_argument(
        "--packet-interval",
        type=int,
        default=500,
        help="Time between two consequent packets, ns",
    )
    parser.add_argument(
        "--flows", default="1-to-1", help="Flows: 1-to-1 or all-to-all"
    ),

    args = parser.parse_args()

    # Validate inputs
    if args.senders < 1:
        print("Error: Number of senders must be at least 1", file=sys.stderr)
        sys.exit(1)
    if args.receivers < 1:
        print("Error: Number of receivers must be at least 1", file=sys.stderr)
        sys.exit(1)
    if args.switches > 2:
        print("Warning: Number of switches should be less than 3", file=sys.stderr)
    return args


def main():
    # Parse command line arguments
    args = parse_arguments()

    # Generate topology
    topology = generate_topology(args.senders, args.receivers, args.switches)
    save_yaml(topology, args.topology_dir + args.topology)
    print(
        f"Topology file saved as {args.topology_dir + args.topology} with {args.senders} senders and {args.receivers} receivers"
    )

    # Generate simulation
    simulation = generate_simulation(
        os.path.relpath(args.topology_dir + args.topology, start=args.simulation_dir),
        args.senders,
        args.receivers,
        args.flows,
        args.simulation_time,
        args.packets,
        args.packet_interval,
    )
    save_yaml(simulation, args.simulation_dir + args.simulation)
    print(f"Simulation file saved as {args.simulation}")


if __name__ == "__main__":
    main()
