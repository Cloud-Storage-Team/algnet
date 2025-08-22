import yaml
import argparse
import os

def parse_arguments():
    """Parse and validate command line arguments"""
    parser = argparse.ArgumentParser(
        description="Generate topology and simulation YAML files for the incast scenario."
    )
    parser.add_argument(
        "--flows", type=int, default=3, help="Number of flows"
    ),    
    parser.add_argument(
        "--topology_file",
        default="incast_topology.yml",
        help="Topology config file",
    )
    parser.add_argument(
        "--simulation_file",
        default="incast_simulation.yml",
        help="Simulation config file",
    )
    return parser.parse_args()


def save_yaml(data, filename):
    """Save data as YAML to a file"""
    with open(filename, "w") as f:
        yaml.dump(data, f, sort_keys=False, default_flow_style=False)


def generate_topology(flows):
    topology = {
        "presets": {
            "link" : {"default" : {
                "latency": "5ns",
                "throughput": "100Gbps",
                "ingress_buffer_size": "1024000B",
                "egress_buffer_size": "1024000B",
            }}
        },
        "packet-spraying" : {"type" : "ecmp"},
        "hosts": {},
        "switches": {},
        "links": {}
    }

    link_idx = 1
    senders = []

    # Add senders
    for i in range(0, flows):
        sender = f"sender{i+1}"
        senders.append(sender)
        topology["hosts"][sender] = {}

        # Add link from sender to switch
        topology["links"][f"link{link_idx}"] = {
            "from": sender,
            "to": "switch",
        }
        link_idx += 1        

        # Add link from switch to sender
        topology["links"][f"link{link_idx}"] = {
            "from": "switch",
            "to": sender,
        }
        link_idx += 1        

    # Add receiver
    topology["hosts"]["receiver"] = {}

    # Add link from switch to receiver
    topology["links"][f"link{link_idx}"] = {
        "from": "switch",
        "to": "receiver"
    }
    link_idx += 1    

    # Add link from receiver to switch
    topology["links"][f"link{link_idx}"] = {
        "from": "receiver",
        "to": "switch"
    }
    link_idx += 1    

    # Add switch
    topology["switches"]["switch"] = {"threshold": 0.7}

    return topology, senders


def generate_simulation(
    topology_file,
    senders
):
    """
    Generate a simulation YAML structure with flows between senders and 1 receiver.
    """
    simulation = {
        "topology_config_path": topology_file,
        "connections": {},
        "simulation_time": 500000,
    }

    for i in range(0, len(senders)):
        simulation["connections"][f"conn{i+1}"] = {
            "sender_id": senders[i],
            "receiver_id": "receiver",
            "packets_to_send": 1000,
            "mplb": "round_robin",
            "flows": {
                "flow": {
                    "type": "tcp",
                    "packet_size": 1500,
                    "cc": {
                        "type": "swift",
                        "base_target": "200ns"
                    }
                }
            },
        }
    return simulation


def main():
    # Parse command line arguments
    args = parse_arguments()

    # Generate topology config file
    topology, senders = generate_topology(args.flows)
    save_yaml(topology, args.topology_file)
    print(f"Topology config saved in {args.topology_file}")

    # Generate simulation config file
    simulation = generate_simulation(
        os.path.relpath(
            args.topology_file, start=os.path.dirname(args.simulation_file)
        ),
        senders
    )
    save_yaml(simulation, args.simulation_file)
    print(f"Simulation config saved in {args.simulation_file}")


if __name__ == "__main__":
    main()
