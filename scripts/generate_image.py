import os
import sys

import yaml
import argparse
from graphviz import Digraph

"""
Generates topology image by given path to topology config file
"""


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
        if device_type == "host":
            color = "#2E7D32"  # Dark green
            icon = "🖥️"
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
        label = f"{link_id}\n"\
                f"⏱ {link_info['latency']}\n" \
                f"📶 {link_info['throughput']}\n" \
                f"ingress {link_info['ingress_buffer_size']}\n" \
                f"egress {link_info['egress_buffer_size']}"

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

    directory = os.path.dirname(output_file)
    file_name, extention = os.path.splitext(os.path.basename(output_file))
    # Delete . from the extention beginning
    extention = extention[1:]
    
    # Render the graph
    graph.render(file_name, directory, format=extention, cleanup=True)
    print(f"Generated topology image: {output_file}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Generate topology image by its configuration file."
    )
    parser.add_argument("-c", "--config", help="Path to the topology configuration file", required=True)
    parser.add_argument("-o", "--output", help="Path to the output image", required=True)
    args = parser.parse_args()

    
    generate_topology(args.config, args.output)
