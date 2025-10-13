import os

from common import *
from generators.topology.common import *

def _receiver_names(cfg: dict) -> list[str]:
    n = int(cfg.get("receivers", 1))
    if n < 1:
        raise ValueError("Config 'receivers' must be >= 1")
    if n == 1:
        return ["receiver"]
    return [f"receiver-{i}" for i in range(1, n + 1)]

def generate_topology(config : dict):
    try:
        presets = config["presets"]
        num_switches_per_device = int(config["num_switches_per_device"])
        packet_spraying = config["packet-spraying"]
        depth = int(config["depth"])
    except KeyError as e:
        raise KeyError(f"Config missing value {e}")
    
    sender_name = "sender"
    receiver_names = _receiver_names(config)

    topology = {
        "presets" : presets,
        "packet-spraying" : packet_spraying,
        "hosts": {
            sender_name : {},
            **{name: {} for name in receiver_names},
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

    # Add links between last layer and receiver
    for name in prev_layer_names:
        for rcv in receiver_names:
            link_generator.add_bidirectional_link(name, rcv)
    return topology
    

def main():
    # Parse command line arguments
    args = parse_topology_generator_args(
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
