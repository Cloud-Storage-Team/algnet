import yaml
import argparse
import sys


def generate_topology(
    num_senders,
    num_receivers,
    switch_name="bus",
    link_latency="1ns",
    link_throughput="1Gbps",
):
    topology = {"devices": {}, "links": {}}

    # Add senders
    for i in range(0, num_senders):
        sender_name = f"sender{i}"
        topology["devices"][sender_name] = {"type": "sender"}
        base_index = 2 * i

        # Add link from sender to switch
        link_name = f"link{base_index}"
        topology["links"][link_name] = {
            "from": sender_name,
            "to": switch_name,
            "latency": link_latency,
            "throughput": link_throughput,
        }

        # Add link from switch to sender
        link_name = f"link{base_index + 1}"
        topology["links"][link_name] = {
            "from": switch_name,
            "to": sender_name,
            "latency": link_latency,
            "throughput": link_throughput,
        }

    # Add receivers
    for i in range(0, num_receivers):
        receiver_name = f"receiver{i}"
        topology["devices"][receiver_name] = {"type": "receiver"}
        base_index = 2 * num_senders + 2 * i

        # Add link from switch to receiver
        link_name = f"link{base_index}"
        topology["links"][link_name] = {
            "from": switch_name,
            "to": receiver_name,
            "latency": link_latency,
            "throughput": link_throughput,
        }

        # Add link from receiver to switch
        link_name = f"link{base_index + 1}"
        topology["links"][link_name] = {
            "from": receiver_name,
            "to": switch_name,
            "latency": link_latency,
            "throughput": link_throughput,
        }

    # Add the switch
    topology["devices"][switch_name] = {"type": "switch"}

    return topology


def generate_simulation(
    topology_path,
    num_senders,
    num_receivers,
    packet_size=1024,
    packet_interval=100,
    algorithm="basic",
    simulation_time=1000000,
):
    """
    Generate a simulation YAML structure with flows between senders and receivers.
    """
    simulation = {
        "topology_config_path": topology_path,
        "flows": {},
        "algorithm": algorithm,
        "simulation_time": simulation_time,
    }

    # Create flows - each sender connects to one receiver (1:1 mapping)
    # If there are more senders than receivers, extra senders will connect to last receiver
    # If there are more receivers than senders, extra receivers won't have flows
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
            }

    return simulation


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
        "--topology",
        default="stress_topology.yml",
        help="Output filename for topology file",
    )
    parser.add_argument(
        "--simulation",
        default="stress_simulation.yml",
        help="Output filename for simulation file",
    )
    parser.add_argument(
        "--topology-path",
        default="../topology_examples/stress_topology.yml",
        help="Path to topology file as referenced in simulation file",
    )
    parser.add_argument(
        "--simulation-path",
        default="../simulation_examples/stress_simulation.yml",
        help="Path to topology file as referenced in simulation file",
    )

    args = parser.parse_args()

    # Validate inputs
    if args.senders < 1:
        print("Error: Number of senders must be at least 1", file=sys.stderr)
        sys.exit(1)
    if args.receivers < 1:
        print("Error: Number of receivers must be at least 1", file=sys.stderr)
        sys.exit(1)

    return args


def main():
    # Parse command line arguments
    args = parse_arguments()

    # Generate topology
    topology = generate_topology(args.senders, args.receivers)
    save_yaml(topology, args.topology_path)
    print(
        f"Topology file saved as {args.topology} with {args.senders} senders and {args.receivers} receivers"
    )

    # Generate simulation
    simulation = generate_simulation(args.topology_path, args.senders, args.receivers)
    save_yaml(simulation, args.simulation_path)
    print(f"Simulation file saved as {args.simulation}")


if __name__ == "__main__":
    main()
