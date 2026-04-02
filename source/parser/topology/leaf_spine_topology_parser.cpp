#include "leaf_spine_topology_parser.hpp"

#include "common.hpp"
#include "host/host_parser.hpp"
#include "link/link_parser.hpp"
#include "switch/switch_parser.hpp"

namespace sim {

Topology parse_leaf_spine_topology(const ConfigNodeWithPreset& node) {
    return Topology(
        parse_leaf_spine_topology_context(node).to_topology_context());
}

TopologyContext LeafSpineTopologyContext::to_topology_context() const {
    TopologyContext ctx;

    auto merge_storage_with_id_table = [](const auto& storage, auto& id_table) {
        for (auto el : storage) {
            id_table.add(el);
        }
    };

    merge_storage_with_id_table(hosts, ctx.hosts_table);

    merge_storage_with_id_table(leaf_switches, ctx.switches_table);
    merge_storage_with_id_table(spine_switches, ctx.switches_table);

    ctx.links_table = links;
    return ctx;
}

LeafSpineTopologyContext parse_leaf_spine_topology_context(
    const ConfigNodeWithPreset& node) {
    const ConfigNodeWithPreset& packet_spraying =
        node["packet-spraying"].value_or_throw();

    const ConfigNodeWithPreset& switches = node["switches"].value_or_throw();
    const ConfigNodeWithPreset& spine_switches =
        switches["spine"].value_or_throw();
    const ConfigNodeWithPreset& leaf_switches =
        switches["leaf"].value_or_throw();

    const ConfigNodeWithPreset& hosts = node["hosts"].value_or_throw();

    const ConfigNodeWithPreset& links = node["links"].value_or_throw();
    const ConfigNodeWithPreset& leaf_spine_links =
        links["leaf-spine"].value_or_throw();
    const ConfigNodeWithPreset& host_leaf_links =
        links["leaf-spine"].value_or_throw();

    LeafSpineParams params{packet_spraying, spine_switches,   leaf_switches,
                           hosts,           leaf_spine_links, host_leaf_links};
    return create_leaf_spine_topology_context(params);
}

static std::string create_name(const std::string& name_prefix,
                               const std::string& name) {
    return name_prefix + name;
}

static std::vector<std::shared_ptr<ISwitch>> create_spine_switches(
    const ConfigNodeWithPreset& switches_params,
    const ConfigNodeWithPreset& packet_spraying,
    const std::string& name_prefix) {
    std::vector<std::shared_ptr<ISwitch>> switches;

    std::size_t count =
        switches_params["count"].as<std::size_t>().value_or_throw();

    for (std::size_t i = 1; i <= count; i++) {
        std::shared_ptr<ISwitch> swtch = parse_switch(
            switches_params, packet_spraying.get_node(),
            create_name(name_prefix, fmt::format("spine-switch-{}", i)));
        switches.emplace_back(swtch);
    }
    return switches;
}

static std::vector<std::shared_ptr<ISwitch>> create_leaf_switches(
    const ConfigNodeWithPreset& switches_params,
    const ConfigNodeWithPreset& packet_spraying,
    const std::string& name_prefix) {
    std::vector<std::shared_ptr<ISwitch>> switches;

    std::size_t count =
        switches_params["count"].as<std::size_t>().value_or_throw();

    for (std::size_t i = 1; i <= count; i++) {
        std::shared_ptr<ISwitch> swtch = parse_switch(
            switches_params, packet_spraying.get_node(),
            create_name(name_prefix, fmt::format("leaf-switch-{}", i)));
        switches.emplace_back(swtch);
    }
    return switches;
}

static std::vector<std::shared_ptr<IHost>> create_hosts(
    const ConfigNodeWithPreset& hosts_params, std::size_t count,
    const std::string& name_prefix) {
    std::vector<std::shared_ptr<IHost>> hosts;

    for (std::size_t i = 1; i <= count; i++) {
        std::shared_ptr<IHost> host = parse_host(
            hosts_params, create_name(name_prefix, fmt::format("host-{}", i)));
        hosts.emplace_back(host);
    }
    return hosts;
}

utils::IdTable<ILink> create_links(
    const std::vector<std::shared_ptr<ISwitch>>& spine_switches,
    const std::vector<std::shared_ptr<ISwitch>>& leaf_switches,
    const std::vector<std::shared_ptr<IHost>>& hosts,
    const ConfigNodeWithPreset& host_leaf_links,
    const ConfigNodeWithPreset& leaf_spine_links) {
    utils::IdTable<ILink> links;
    // add links between hosts & leaf switches
    for (std::size_t i = 0; i < leaf_switches.size(); i++) {
        add_links_between(hosts[i], leaf_switches[i], host_leaf_links, links);
    }
    // add links between leaf & spine switches
    for (auto leaf_switch : leaf_switches) {
        for (auto spine_switch : spine_switches) {
            add_links_between(leaf_switch, spine_switch, leaf_spine_links,
                              links);
        }
    }
    return links;
}

LeafSpineTopologyContext create_leaf_spine_topology_context(
    LeafSpineParams params, const std::string& names_prefix) {
    LeafSpineTopologyContext ctx;

    ctx.spine_switches = create_spine_switches(
        params.spine_switches, params.packet_spraying, names_prefix);

    ctx.leaf_switches = create_leaf_switches(
        params.leaf_switches, params.packet_spraying, names_prefix);

    ctx.hosts =
        create_hosts(params.hosts, ctx.leaf_switches.size(), names_prefix);

    ctx.links = create_links(ctx.spine_switches, ctx.leaf_switches, ctx.hosts,
                             params.host_leaf_links, params.leaf_spine_links);
    return ctx;
}

}  // namespace sim