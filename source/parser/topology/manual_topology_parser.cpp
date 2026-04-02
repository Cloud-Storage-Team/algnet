#include "manual_topology_parser.hpp"

#include "host/host_parser.hpp"
#include "link/link_parser.hpp"
#include "switch/switch_parser.hpp"

namespace sim {

static utils::IdTable<IHost> parse_hosts(
    const std::optional<ConfigNode>& host_presets_node,
    const ConfigNode& hosts_node);

static utils::IdTable<ISwitch> parse_switches(
    const std::optional<ConfigNode>& switches_presets_node,
    const ConfigNodeWithPreset& packet_spraying_node,
    const ConfigNode& switches_node);

static utils::IdTable<ILink> parse_links(
    const std::optional<ConfigNode>& link_presets_node,
    const ConfigNode& links_node, const utils::IdTable<IDevice>& device_table);

static utils::StrExpected<utils::IdTable<IDevice> > build_device_table(
    const utils::IdTable<IHost>& hosts_table,
    const utils::IdTable<ISwitch>& switches_table);

Topology parse_manual_topology(const ConfigNode& node) {
    std::optional<ConfigNode> presets_node = node["presets"].to_optional();
    ConfigNodeWithPreset node_with_preset(node, presets_node);

    TopologyContext ctx;
    {
        // hosts parsing

        ConfigNode hosts_node = node["hosts"].value_or_throw();
        ctx.hosts_table = parse_hosts(presets_node, hosts_node);
    }

    {
        // switches parsing

        const ConfigNodeWithPreset& packet_spraying_node =
            node_with_preset["packet-spraying"].value_or_throw();

        ConfigNode switches_node = node["switches"].value_or_throw();
        ctx.switches_table =
            parse_switches(presets_node, packet_spraying_node, switches_node);
    }

    {
        // links parsing

        utils::IdTable<IDevice> device_table =
            build_device_table(ctx.hosts_table, ctx.switches_table)
                .value_or_throw();

        ConfigNode links_node = node["links"].value_or_throw();
        ctx.links_table = parse_links(presets_node, links_node, device_table);
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
        Id host_id = host->get_id();
        if (!hosts_table.emplace(host_id, host).second) {
            throw hosts_node.create_parsing_error(
                fmt::format("Two hosts with same name: {}", host_id));
        }
    }
    return hosts_table;
}

static utils::IdTable<ISwitch> parse_switches(
    const std::optional<ConfigNode>& switches_presets_node,
    const ConfigNodeWithPreset& packet_spraying_node,
    const ConfigNode& switches_node) {
    utils::IdTable<ISwitch> switches_table;
    for (const auto& switch_node : switches_node) {
        ConfigNodeWithPreset switch_node_with_preset(switch_node,
                                                     switches_presets_node);
        std::shared_ptr<ISwitch> swtch = SwitchParser::parse_i_switch(
            switch_node_with_preset, packet_spraying_node);
        Id switch_id = swtch->get_id();
        if (!switches_table.emplace(switch_id, swtch).second) {
            throw switches_node.create_parsing_error(
                fmt::format("Two switches with same name: {}", switch_id));
        }
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
            return std::unexpected(fmt::format(
                "Could not add switch named {} to device table: device with "
                "such name already exists",
                switch_name));
        }
    }

    return device_table;
}

static utils::IdTable<ILink> parse_links(
    const std::optional<ConfigNode>& link_presets_node,
    const ConfigNode& links_node, const utils::IdTable<IDevice>& device_table) {
    utils::IdTable<ILink> links_table;
    for (const auto& link_node : links_node) {
        ConfigNodeWithPreset link_node_with_preset(link_node,
                                                   link_presets_node);
        std::shared_ptr<ILink> link =
            parse_i_link(link_node_with_preset, device_table);
        Id link_id = link->get_id();
        if (!links_table.emplace(link->get_id(), link).second) {
            throw links_node.create_parsing_error(
                fmt::format("Two links with same name: {}", link_id));
        }
    }
    return links_table;
}

}  // namespace sim
