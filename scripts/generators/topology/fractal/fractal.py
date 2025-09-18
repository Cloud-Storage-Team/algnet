import os

from generators.common import *
from generators.topology.common import *

def generate_topology(config : dict):
    try:
        presets = config["presets"]
        num_switches_per_device = int(config["num_switches_per_device"])
        packet_spraying = config["packet-spraying"]
        depth = int(config["depth"])
    except KeyError as e:
        raise KeyError(f"Config missing value {e}")
    
    sender_name = "sender"
    receiver_name = "receiver"
    topology = {
        "presets" : presets,
        "packet-spraying" : packet_spraying,
        "hosts": {
            sender_name : {},
            receiver_name : {}
        },
        "switches":{},
        "links": {}
    }

    link_generator = LinkGenerator(topology)

    switch_nums_range = range(1, num_switches_per_device + 1)

    prev_layer_names = [sender_name]
    current_layer_names = [[f"switch-{i}" for i in switch_nums_range]]
    # Add layers
    for _ in range (depth):
        for i, prev_layer_name in enumerate(prev_layer_names):
            for switch_name in current_layer_names[i]:
                topology["switches"][switch_name] = {"type" : "switch"}

                link_generator.add_bidirectional_link(prev_layer_name, switch_name)

        prev_layer_names = [name for names in current_layer_names for name in names]
        current_layer_names = [[f"{name}-{i}" for i in switch_nums_range] for name in prev_layer_names]

    # Add links betwwen last layer and receiver
    for name in prev_layer_names:
        link_generator.add_bidirectional_link(name, receiver_name)
    return topology
    

def main():
    # Parse command line arguments
    args = parse_args(
        os.path.join(os.path.dirname(__file__), "default_config.yml"),
        "Generator of fractal topology. For more information see README.md near this script"
    )

    config = load_yaml(args.config)
    # Generate topology
    topology = generate_topology(config)

    save_yaml(topology, args.output_path)
    print(
        f"Topology file saved as {args.output_path}"
    )


if __name__ == "__main__":
    main()
