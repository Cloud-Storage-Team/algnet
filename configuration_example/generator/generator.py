import yaml
from graphviz import Digraph
import os, sys


def generate_topology(config_file, output_file, picture_label='Network Topology'):
    # Load the YAML configuration
    with open(config_file) as f:
        config = yaml.safe_load(f)

    # Create a directed graph with styling
    graph = Digraph(engine='dot')
    graph.attr(rankdir='LR',
               bgcolor='#F5F5F5',
               fontname='Helvetica',
               label=picture_label,
               fontsize='20',
               pad='0.5',
               nodesep='0.5',
               ranksep='1.2')

    # Common style parameters
    node_style = {
        'style': 'filled,rounded',
        'fontname': 'Helvetica',
        'fontsize': '12',
        'penwidth': '2'
    }

    edge_style = {
        'fontname': 'Helvetica',
        'fontsize': '10',
        'penwidth': '2'
    }

    # Add hosts with icons and styling
    hosts = config.get('hosts', {})
    for host_id, host_info in hosts.items():
        host_type = host_info.get('type', '')
        if host_type == 'sender':
            shape = 'oval'
            color = '#2E7D32'  # Dark green
            icon = '🖥️'
        else:
            shape = 'oval'
            color = '#C62828'  # Dark red
            icon = '🖳'

        label = f'''<
            <table border="0" cellborder="0">
                <tr><td><font face="Arial" point-size="14">{icon}</font></td></tr>
                <tr><td><b>{host_info['name']}</b></td></tr>
            </table>
        >'''

        graph.node(host_id,
                   label=label,
                   shape='none',
                   color=color,
                   fillcolor=f'{color}20',
                   **node_style)

    # Add switches with 3D effect
    switches = config.get('switches', {})
    for switch_id, switch_info in switches.items():
        label = f'''<
            <table border="0" cellborder="0">
                <tr><td><font face="Arial" point-size="14">🌐</font></td></tr>
                <tr><td><b>{switch_info['name']}</b></td></tr>
            </table>
        >'''

        graph.node(switch_id,
                   label=label,
                   shape='box3d',
                   color='#1565C0',  # Dark blue
                   fillcolor='#64B5F620',
                   **node_style)

    # Add styled links with metrics
    links = config.get('links', {})
    for link_id, link_info in links.items():
        from_node = link_info['from']
        to_node = link_info['to']
        label = f"⏱ {link_info['latency']}\n📶 {link_info['throughput']}"

        graph.edge(from_node, to_node,
                   label=label,
                   color='#616161',
                   arrowhead='open',
                   arrowsize='0.8',
                   fontcolor='#424242',
                   **edge_style)

    # Create invisible edges to enforce hierarchy
    with graph.subgraph() as s:
        s.attr(rank='same')
        for host_id in hosts:
            if hosts[host_id]['type'] == 'sender':
                s.node(host_id)

    with graph.subgraph() as s:
        s.attr(rank='same')
        for host_id in hosts:
            if hosts[host_id]['type'] == 'receiver':
                s.node(host_id)

    # Render the graph
    graph.render(output_file, format='svg', cleanup=True)
    print(f"Generated topology image: {output_file}.svg")


if __name__ == '__main__':

    print(sys.argv)
    if len(sys.argv) != 2:
        print('Usage: python <program> <catalog>')
        exit(1)

    dir_name = sys.argv[1]
    for file in os.listdir(dir_name):
        name = file.split('.')[0]
        pretty_name = ' '.join(map(lambda x: x.capitalize(), name.split('_')))
        generate_topology(os.path.join(dir_name, file), name, picture_label=pretty_name)
