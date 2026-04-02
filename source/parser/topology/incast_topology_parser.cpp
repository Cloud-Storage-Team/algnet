#include "incast_topology_parser.hpp"

#include "common.hpp"
#include "host/host_parser.hpp"
#include "switch/switch_parser.hpp"

namespace sim {

Topology parse_incast_topology(const ConfigNodeWithPreset& node) {
    return Topology(parse_incast_topology_context(node).to_topology_context());
}

TopologyContext IncastTopologyContext::to_topology_context() const {
    TopologyContext ctx;
    ctx.hosts_table = senders;
    ctx.hosts_table.add(receiver);

    ctx.switches_table.add(swtch);
    ctx.links_table = links;
    return ctx;
}

IncastTopologyContext parse_incast_topology_context(
    const ConfigNodeWithPreset& node) {
    // create senders
    IncastTopologyContext ctx;

    const ConfigNodeWithPreset& senders_node = node["senders"].value_or_throw();
    std::size_t senders_count =
        senders_node["count"].as<std::size_t>().value_or_throw();

    for (std::size_t i = 1; i <= senders_count; i++) {
        Id sender_id = fmt::format("sender-{}", i);
        std::shared_ptr<IHost> sender = parse_host(senders_node, sender_id);
        ctx.senders.add(sender);
    }

    // create receiver
    const ConfigNodeWithPreset& receiver_preset =
        node["receiver"].value_or_throw();

    Id receiver_id = "receiver";
    ctx.receiver = parse_host(receiver_preset, receiver_id);

    // create switch
    const ConfigNodeWithPreset& switch_node = node["switch"].value_or_throw();
    const ConfigNodeWithPreset& packet_spraying_node =
        node["packet-spraying"].value_or_throw();

    Id switch_id = "switch";
    ctx.swtch = parse_switch(switch_node, packet_spraying_node, switch_id);

    const ConfigNodeWithPreset& links_preset = node["links"].value_or_throw();

    // create links from between senders and switch
    for (const auto& [_, sender] : ctx.senders) {
        add_links_between(sender, ctx.swtch, links_preset, ctx.links);
    }

    const ConfigNodeWithPreset& bottleneck_link_preset =
        ConfigNodeWithPreset(node)["bottleneck-link"].value_or(links_preset);

    add_links_between(ctx.swtch, ctx.receiver, bottleneck_link_preset,
                      ctx.links);

    return ctx;
}

}  // namespace sim