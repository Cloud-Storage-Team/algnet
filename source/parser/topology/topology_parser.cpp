#include "topology_parser.hpp"

#include "host/host_parser.hpp"
#include "link/new_link_parser.hpp"
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
    const ConfigNode& links_node, const utils::IdTable<IDevice> device_table);

static utils::StrExpected<utils::IdTable<IDevice> > build_device_table(
    const utils::IdTable<IHost>& hosts_table,
    const utils::IdTable<ISwitch>& switches_table);

Topology parse_topology(const ConfigNode& node) {
    std::optional<ConfigNode> presets_node = node["presets"].to_optional();

    TopologyContext ctx;
    {
        // hosts parsing
        std::optional<ConfigNode> host_presets_node =
            (presets_node ? presets_node.value()["host"].to_optional()
                          : std::nullopt);

        ConfigNode hosts_node = node["hosts"].value_or_throw();
        ctx.hosts_table = parse_hosts(host_presets_node, hosts_node);
    }

    {
        // switches parsing
        std::optional<ConfigNode> switches_presets_node =
            (presets_node ? presets_node.value()["switch"].to_optional()
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
            (presets_node ? presets_node.value()["link"].to_optional()
                          : std::nullopt);

        utils::IdTable<IDevice> device_table =
            build_device_table(ctx.hosts_table, ctx.switches_table)
                .value_or_throw();

        ConfigNode links_node = node["links"].value_or_throw();
        ctx.links_table =
            parse_links(links_presets_node, links_node, device_table);
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

static utils::StrExpected<utils::IdTable<IDevice> > build_device_table(
    const utils::IdTable<IHost>& hosts_table,
    const utils::IdTable<ISwitch>& switches_table) {
    utils::IdTable<IDevice> device_table;
    for (const auto& [host_name, host] : hosts_table) {
        device_table.emplace(host_name, host);
    }

    for (const auto& [switch_name, swtch] : switches_table) {
        if (!device_table.emplace(switch_name, swtch).second) {
            return std::unexpected(
                "Could not add switch named {} to device table: device with "
                "such name already exists");
        }
    }

    return device_table;
}

static utils::IdTable<ILink> parse_links(
    const std::optional<ConfigNode>& link_presets_node,
    const ConfigNode& links_node, const utils::IdTable<IDevice> device_table) {
    utils::IdTable<ILink> links_table;
    for (const auto& link_node : links_node) {
        ConfigNodeWithPreset link_node_with_preset(link_node,
                                                   link_presets_node);
        std::shared_ptr<ILink> link =
            new_parse_i_link(link_node_with_preset, device_table);
        links_table[link->get_id()] = link;
    }
    return links_table;
}

}  // namespace sim
