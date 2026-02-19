#include "topology_parser.hpp"

#include "host/host_parser.hpp"
#include "link/link_parser.hpp"
#include "switch/switch_parser.hpp"

namespace sim {

static utils::IdTable<IHost> parse_hosts(
    const std::optional<ConfigNode>& host_presets_node,
    const ConfigNode& hosts_node);

static utils::IdTable<ISwitch> parse_switches(
    const std::optional<ConfigNode>& switches_presets_node,
    const ConfigNode& packet_spraying_node, const ConfigNode& switches_node);

static utils::IdTable<ILink> parse_links(
    const std::optional<ConfigNode>& link_presets_node,
    const ConfigNode& links_node);

Topology parse_topology(const ConfigNode& node) {
    std::optional<ConfigNode> presets_node = node["presets"].to_optional();

    TopologyContext ctx;
    {
        // hosts parsing
        std::optional<ConfigNode> host_presets_node =
            (presets_node ? presets_node.value()["hosts"].to_optional()
                          : std::nullopt);

        ConfigNode hosts_node = node["hosts"].value_or_throw();
        ctx.hosts_table = parse_hosts(host_presets_node, hosts_node);
    }

    {
        // switches parsing
        std::optional<ConfigNode> switches_presets_node =
            (presets_node ? presets_node.value()["switches"].to_optional()
                          : std::nullopt);

        ConfigNode packet_spraying_node =
            node["packet-spraying"].value_or_throw();

        ConfigNode switches_node = node["switches"].value_or_throw();
        ctx.switches_table = parse_switches(
            switches_presets_node, packet_spraying_node, switches_node);
    }
    {
        // links parsing
        std::optional<ConfigNode> links_presets_node =
            (presets_node ? presets_node.value()["links"].to_optional()
                          : std::nullopt);

        ConfigNode links_node = node["links"].value_or_throw();
        ctx.links_table = parse_links(links_presets_node, links_node);
    }

    return Topology(std::move(ctx));
}

static utils::IdTable<IHost> parse_hosts(
    const std::optional<ConfigNode>& host_presets_node,
    const ConfigNode& hosts_node) {
    utils::IdTable<IHost> hosts_table;
    for (const auto& host_node : hosts_node) {
        ConfigNodeWithPreset host_node_with_preset(host_node,
                                                   host_presets_node);
        std::shared_ptr<IHost> host =
            HostParser::parse_i_host(host_node_with_preset);
        hosts_table[host->get_id()] = host;
    }
    return hosts_table;
}

static utils::IdTable<ISwitch> parse_switches(
    const std::optional<ConfigNode>& switches_presets_node,
    const ConfigNode& packet_spraying_node, const ConfigNode& switches_node) {
    utils::IdTable<ISwitch> switches_table;
    for (const auto& switch_node : switches_node) {
        ConfigNodeWithPreset switch_node_with_preset(switch_node,
                                                     switches_presets_node);
        std::shared_ptr<ISwitch> swtch = SwitchParser::parse_i_switch(
            switch_node_with_preset, packet_spraying_node);
        switches_table[swtch->get_id()] = swtch;
    }
    return switches_table;
}

static utils::IdTable<ILink> parse_links(
    const std::optional<ConfigNode>& link_presets_node,
    const ConfigNode& links_node) {
    utils::IdTable<ILink> links_table;
    for (const auto& link_node : links_node) {
        ConfigNodeWithPreset link_node_with_preset(link_node,
                                                   link_presets_node);
        std::shared_ptr<ILink> link =
            LinkParser::parse_i_link(link_node_with_preset);
        links_table[link->get_id()] = link;
    }
    return links_table;
}

}  // namespace sim
