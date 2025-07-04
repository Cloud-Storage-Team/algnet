import os
import sys

import yaml
from graphviz import Digraph


def generate_topology(config_file, output_file, picture_label="Network Topology"):
    # Load the YAML configuration
    with open(config_file) as f:
        config = yaml.safe_load(f)

    # Create a directed graph with top-to-bottom layout
    graph = Digraph(engine="dot")
    graph.attr(
        rankdir="TB",
        bgcolor="#F5F5F5",
        fontname="Helvetica",
        label=picture_label,
        fontsize="20",
        pad="0.5",
        nodesep="0.8",
        ranksep="1.5",
    )

    # Common style parameters
    node_style = {
        "style": "filled,rounded",
        "fontname": "Helvetica",
        "fontsize": "12",
        "penwidth": "2",
    }

    edge_style = {"fontname": "Helvetica", "fontsize": "10", "penwidth": "2"}

    # Add hosts with vertical alignment
    devices = config.get("devices", {})
    for device_id, device_info in devices.items():
        node_shape = "none"

        device_type = device_info.get("type", "")
        if device_type == "sender":
            color = "#2E7D32"  # Dark green
            icon = "🖥️"
        elif device_type == "receiver":
            color = "#C62828"  # Dark red
            icon = "🖳"
        elif device_type == "switch":
            color = "#1565C0"  # Dark blue
            icon = "🌐"
            node_shape = "box3d"
        else:
            raise ValueError(f"Unknown device type: {device_type}")

        label = f"""<
            <table border="0" cellborder="0">
                <tr><td><font face="Arial" point-size="14">{icon}</font></td></tr>
                <tr><td><b>{device_id}</b></td></tr>
            </table>
        >"""

        graph.node(
            device_id,
            label=label,
            shape=node_shape,
            color=color,
            fillcolor=f"{color}20",
            **node_style,
        )

    # Add styled links with metrics
    links = config.get("links", {})
    for link_id, link_info in links.items():
        from_node = link_info["from"]
        to_node = link_info["to"]
        label = f"⏱ {link_info['latency']}\n📶 {link_info['throughput']}\ningress {link_info['ingress_buffer_size']}\negress {link_info['egress_buffer_size']}"

        graph.edge(
            from_node,
            to_node,
            label=label,
            color="#616161",
            arrowhead="open",
            arrowsize="0.8",
            fontcolor="#424242",
            **edge_style,
        )

    # Create hierarchical groups
    with graph.subgraph() as s:
        s.attr(rank="min")
        for device_id in devices:
            if devices[device_id]["type"] == "sender":
                s.node(device_id)

    with graph.subgraph() as s:
        s.attr(rank="max")
        for device_id in devices:
            if devices[device_id]["type"] == "receiver":
                s.node(device_id)

    # Render the graph
    graph.render(output_file, format="svg", cleanup=True)
    print(f"Generated topology image: {output_file}.svg")


"""
Usage example:
python3 generator.py plots ../topology_examples/bus_topology.yml
"""

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print(f"Usage: python {sys.argv[0]} <output-directory> <filenames>...")
        exit(1)

    dir_name = sys.argv[1]
    for idx in range(2, len(sys.argv)):
        path = sys.argv[idx]
        filename = os.path.basename(path)
        filename = filename.split(".")[0]
        pretty_image_name = " ".join(map(lambda x: x.capitalize(), filename.split("_")))
        generate_topology(
            path, os.path.join(dir_name, filename), picture_label=pretty_image_name
        )
