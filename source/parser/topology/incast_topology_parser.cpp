#include "incast_topology_parser.hpp"

#include "common.hpp"
#include "host/host_parser.hpp"
#include "switch/switch_parser.hpp"

namespace sim {

Topology parse_incast_topology(const ConfigNode& node) {
    // create senders
    utils::IdTable<IHost> senders;
    ConfigNodeWithPreset node_with_preset(node);

    const ConfigNodeWithPreset& senders_node =
        node_with_preset["senders"].value_or_throw();
    std::size_t senders_count =
        senders_node["count"].as<std::size_t>().value_or_throw();

    for (std::size_t i = 1; i <= senders_count; i++) {
        Id sender_id = fmt::format("sender-{}", i);
        std::shared_ptr<IHost> sender = parse_host(senders_node, sender_id);
        senders.emplace(sender_id, sender);
    }

    // create receiver
    const ConfigNodeWithPreset& receiver_preset =
        node_with_preset["receiver"].value_or_throw();

    Id receiver_id = "receiver";
    std::shared_ptr<IHost> receiver = parse_host(receiver_preset, receiver_id);

    // create switch
    const ConfigNodeWithPreset& switch_node =
        node_with_preset["switch"].value_or_throw();
    const ConfigNode& packet_spraying_node =
        node["packet-spraying"].value_or_throw();

    Id switch_id = "switch";
    std::shared_ptr<ISwitch> swtch =
        parse_switch(switch_node, packet_spraying_node, switch_id);

    const ConfigNodeWithPreset& links_preset =
        node_with_preset["links"].value_or_throw();

    utils::IdTable<ILink> links;

    // create links from between senders and switch
    for (const auto& [_, sender] : senders) {
        add_links_between(sender, swtch, links_preset, links);
    }

    const ConfigNodeWithPreset& bottleneck_link_preset =
        ConfigNodeWithPreset(node)["bottleneck-link"].value_or(links_preset);

    add_links_between(swtch, receiver, bottleneck_link_preset, links);

    TopologyContext ctx;

    ctx.hosts_table = std::move(senders);
    ctx.hosts_table.emplace(receiver->get_id(), receiver);

    ctx.switches_table.emplace(swtch->get_id(), swtch);

    ctx.links_table = std::move(links);

    return Topology(ctx);
}

}  // namespace sim