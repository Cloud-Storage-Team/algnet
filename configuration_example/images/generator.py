import yaml
import json

def generate_html_from_yaml(yaml_file, output_html):
    with open(yaml_file, 'r') as file:
        data = yaml.safe_load(file)

    nodes = []
    links = []

    for host, details in data.get('hosts', {}).items():
        nodes.append({"id": host, "type": details['type']})

    for switch, details in data.get('switches', {}).items():
        nodes.append({"id": switch, "type": details['type']})

    for link, details in data.get('links', {}).items():
        links.append({"source": details['from'], "target": details['to']})

    graph_data = {
        "nodes": nodes,
        "links": links
    }

    template_insert = '(${d.x},${d.y})'

    html_template = f"""<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Datacenter Network Topology</title>
    <script src="https://d3js.org/d3.v6.min.js"></script>
    <style>
        .node circle {{ fill: steelblue; stroke: white; stroke-width: 2px; }}
        .link {{ stroke: #999; stroke-opacity: 0.6; }}
        text {{ font-family: Arial, sans-serif; font-size: 12px; fill: black; }}
    </style>
</head>
<body>
    <svg width="800" height="600"></svg>
    <script>
        const graph = {json.dumps(graph_data, indent=4)};
        const svg = d3.select("svg"), width = +svg.attr("width"), height = +svg.attr("height");
        
        const simulation = d3.forceSimulation(graph.nodes)
            .force("link", d3.forceLink(graph.links).id(d => d.id).distance(100))
            .force("charge", d3.forceManyBody().strength(-300))
            .force("center", d3.forceCenter(width / 2, height / 2));
        
        const link = svg.append("g")
            .selectAll("line")
            .data(graph.links)
            .enter().append("line")
            .attr("class", "link")
            .attr("stroke-width", 2);
        
        const node = svg.append("g")
            .selectAll("g")
            .data(graph.nodes)
            .enter().append("g");
        
        node.append("circle")
            .attr("r", 10)
            .attr("fill", d => d.type === "switch" ? "orange" : "steelblue");
        
        node.append("text")
            .attr("x", 12)
            .attr("y", 3)
            .text(d => d.id);
        
        node.call(d3.drag()
            .on("start", dragstarted)
            .on("drag", dragged)
            .on("end", dragended));
        
        simulation.on("tick", () => {{
            link.attr("x1", d => d.source.x)
                .attr("y1", d => d.source.y)
                .attr("x2", d => d.target.x)
                .attr("y2", d => d.target.y);
            node.attr("transform", d => `translate{template_insert}`);
        }});
        
        function dragstarted(event, d) {{
            if (!event.active) simulation.alphaTarget(0.3).restart();
            d.fx = d.x;
            d.fy = d.y;
        }}
        function dragged(event, d) {{ d.fx = event.x; d.fy = event.y; }}
        function dragended(event, d) {{
            if (!event.active) simulation.alphaTarget(0);
            d.fx = null;
            d.fy = null;
        }}
    </script>
</body>
</html>"""

    with open(output_html, 'w') as file:
        file.write(html_template)

root = 'D:\\Diploma\\Code\\'

names = {
    'fat_tree_topology',
    'mesh_topology',
    'star_topology',
    'topology_simple',
    'three_tier_topology'
}
for name in names:
    generate_html_from_yaml(root + 'configuration_example\\examples\\' + name + '.yml', root + 'configuration_example\\images\\' + name + '.html')
# generate_html_from_yaml(root + 'configuration_example\\examples\\bus_topology.yml', root + 'configuration_example\\images\\bus_topology.html')

# Example usage
# generate_html_from_yaml('datacenter.yaml', 'datacenter_topology.html')
