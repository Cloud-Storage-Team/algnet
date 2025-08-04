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
    topology = {
        "hosts": {},
        "switches": {},
        "links": {}
    }
    base_index = 0

    if switch_name == "sender" or switch_name == "receiver":
        raise ValueError("Switch name cannot be 'sender' or 'receiver'")

    switch_names = [f"{switch_name}_{i}" for i in range(num_switches)]
    sender_switch_name = switch_names[0]
    receiver_switch_name = switch_names[-1]

    sender_names = []
    receiver_names = []

    # Add senders
    for i in range(0, num_senders):
        sender_name = f"sender_{i}"
        sender_names.append(sender_name)
        topology["hosts"][sender_name] = {}

        # Add link from sender to switch
        link_name = f"link_{base_index}"
        base_index += 1
        topology["links"][link_name] = {
            "from": sender_name,
            "to": sender_switch_name,
            "latency": link_latency,
            "throughput": link_throughput,
            "ingress_buffer_size": ingress_buffer_size,
            "egress_buffer_size": egress_buffer_size,
        }

        # Add link from switch to sender
        link_name = f"link_{base_index}"
        base_index += 1
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
        receiver_name = f"receiver_{i}"
        receiver_names.append(receiver_name)
        topology["hosts"][receiver_name] = {}

        # Add link from switch to receiver
        link_name = f"link_{base_index}"
        base_index += 1
        topology["links"][link_name] = {
            "from": receiver_switch_name,
            "to": receiver_name,
            "latency": link_latency,
            "throughput": link_throughput,
            "ingress_buffer_size": ingress_buffer_size,
            "egress_buffer_size": egress_buffer_size,
        }

        # Add link from receiver to switch
        link_name = f"link_{base_index}"
        base_index += 1
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
        topology["switches"][switch_names[i]] = {"threshold": 0.7}

    # Add links between switches
    for i in range(0, num_switches - 1):

        # Forward link
        link_name = f"link_{base_index}"
        base_index += 1
        topology["links"][link_name] = {
            "from": switch_names[i],
            "to": switch_names[i + 1],
            "latency": link_latency,
            "throughput": link_throughput,
            "ingress_buffer_size": ingress_buffer_size,
            "egress_buffer_size": egress_buffer_size,
        }

        # Backward link
        link_name = f"link_{base_index}"
        base_index += 1
        topology["links"][link_name] = {
            "from": switch_names[i + 1],
            "to": switch_names[i],
            "latency": link_latency,
            "throughput": link_throughput,
            "ingress_buffer_size": ingress_buffer_size,
            "egress_buffer_size": egress_buffer_size,
        }

    return topology, sender_names, receiver_names


def generate_simulation(
    topology_file,
    sender_names,
    receiver_names,
    flows,
    simulation_time,
    packet_interval,
    number_of_packets,
    packet_size=1500,
    flow_type="tcp",
):
    """
    Generate a simulation YAML structure with flows between senders and receivers.
    """
    simulation = {
        "topology_config_path": topology_file,
        "flows": {},
        "simulation_time": simulation_time,
    }

    cc = {
        "type": "tahoe",
    }

    num_senders = len(sender_names)
    num_receivers = len(receiver_names)

    if flows == "1-to-1":

        # One sender to one receiver
        # If there are more senders than receivers, extra senders will connect to last receiver
        # If there are more receivers than senders, extra receivers won't have flows
        flow_id = 0
        for i in range(0, num_senders):
            flow_name = f"flow_{flow_id}"
            flow_id += 1
            simulation["flows"][flow_name] = {
                "type": flow_type,
                "sender_id": sender_names[i],
                "receiver_id": receiver_names[min(i, num_receivers - 1)],
                "packet_size": packet_size,
                "packet_interval": packet_interval,
                "number_of_packets": number_of_packets,
                "cc": cc
            }
        return simulation

    elif flows == "all-to-all":
        # Each sender to all receivers
        flow_id = 0
        for i in range(0, num_senders):
            for j in range(0, num_receivers):
                flow_name = f"flow_{flow_id}"
                flow_id += 1
                simulation["flows"][flow_name] = {
                    "type": flow_type,
                    "sender_id": sender_names[i],
                    "receiver_id": receiver_names[j],
                    "packet_size": packet_size,
                    "packet_interval": packet_interval,
                    "number_of_packets": number_of_packets,
                    "cc": cc
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
        "--topology-path",
        default="bottleneck_topology.yml",
        help="Path to the output topology config file",
    )
    parser.add_argument(
        "--simulation-path",
        default="bottleneck_simulation.yml",
        help="Path to the output simulation config file",
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
    topology, sender_names, receiver_names = generate_topology(
        args.senders, args.receivers, args.switches
    )

    assert len(receiver_names) == args.receivers and len(sender_names) == args.senders

    save_yaml(topology, args.topology_path)
    print(
        f"Topology file saved as {args.topology_path} with {args.senders} senders and {args.receivers} receivers"
    )

    # Generate simulation
    simulation = generate_simulation(
        os.path.relpath(
            args.topology_path, start=os.path.dirname(args.simulation_path)
        ),
        sender_names,
        receiver_names,
        args.flows,
        args.simulation_time,
        args.packets,
        args.packet_interval,
    )
    save_yaml(simulation, args.simulation_path)
    print(f"Simulation file saved as {args.simulation_path}")


if __name__ == "__main__":
    main()
