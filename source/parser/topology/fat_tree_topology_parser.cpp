#include "fat_tree_topology_parser.hpp"

#include "common.hpp"
#include "host/host_parser.hpp"
#include "link/link_parser.hpp"
#include "switch/switch_parser.hpp"

namespace sim {

Topology parse_fat_tree_topology(const ConfigNodeWithPreset& node) {
    return Topology(
        parse_fat_tree_topology_context(node).to_topology_context());
}

TopologyContext FatTreeTopologyContext::to_topology_context() const {
    TopologyContext ctx;

    // hosts
    for (const auto& host : hosts) {
        ctx.hosts_table.add(host);
    }

    // switches
    for (const auto& swtch : switches.core) {
        ctx.switches_table.add(swtch);
    }
    for (const auto& swtch : switches.aggr) {
        ctx.switches_table.add(swtch);
    }
    for (const auto& swtch : switches.edge) {
        ctx.switches_table.add(swtch);
    }

    // links
    for (const auto& link : links.core_aggr) {
        ctx.links_table.add(link);
    }
    for (const auto& link : links.aggr_edge) {
        ctx.links_table.add(link);
    }
    for (const auto& link : links.edge_host) {
        ctx.links_table.add(link);
    }

    return ctx;
}

FatTreeTopologyContext parse_fat_tree_topology_context(
    const ConfigNodeWithPreset& node) {
    const ConfigNodeWithPreset& packet_spraying_node =
        node["packet-spraying"].value_or_throw();
    const ConfigNodeWithPreset& switches_node =
        node["switches"].value_or_throw();

    const ConfigNodeWithPreset& switches_ports_count =
        switches_node["ports-count"].value_or_throw();

    const ConfigNodeWithPreset& core_switches_node =
        switches_node["core"].value_or_throw();
    const ConfigNodeWithPreset& aggr_switches_node =
        switches_node["aggr"].value_or_throw();
    const ConfigNodeWithPreset& edge_switches_node =
        switches_node["edge"].value_or_throw();

    const ConfigNodeWithPreset& hosts_node = node["hosts"].value_or_throw();

    const ConfigNodeWithPreset& links_node = node["links"].value_or_throw();

    const ConfigNodeWithPreset& core_aggr_links_node =
        links_node["core-aggr"].value_or_throw();
    const ConfigNodeWithPreset& aggr_edge_links_node =
        links_node["aggr-edge"].value_or_throw();
    const ConfigNodeWithPreset& edge_host_links_node =
        links_node["edge-host"].value_or_throw();

    FatTreeParams params{
        packet_spraying_node, switches_ports_count, core_switches_node,
        aggr_switches_node,   edge_switches_node,   hosts_node,
        core_aggr_links_node, aggr_edge_links_node, edge_host_links_node};

    return create_fat_tree_topology_context(params);
}

static void add_aggr_and_below(std::size_t k, std::size_t pod_num,
                               FatTreeParams params,
                               FatTreeTopologyContext& ctx);

FatTreeTopologyContext create_fat_tree_topology_context(FatTreeParams params) {
    std::size_t k = params.switch_ports.as_or_throw<std::size_t>();
    if (k & 1) {
        throw params.switch_ports.create_parsing_error(
            fmt::format("Ports number should be even number but got {}", k));
    }

    std::size_t core_switches_count = k * k / 4;
    FatTreeTopologyContext ctx;
    for (std::size_t i = 1; i <= core_switches_count; i++) {
        ctx.switches.core.emplace_back(parse_switch(params.core_switches,
                                                    params.packet_spraying,
                                                    fmt::format("core-{}", i)));
    }

    std::size_t pods_count = k;

    for (std::size_t pod_num = 1; pod_num <= pods_count; pod_num++) {
        add_aggr_and_below(k, pod_num, params, ctx);
    }
    return ctx;
}

static void add_hosts_and_their_links(std::size_t hosts_count,
                                      std::shared_ptr<ISwitch> edge,
                                      FatTreeParams params,
                                      FatTreeTopologyContext& ctx) {
    for (std::size_t host_num = 1; host_num <= hosts_count; host_num++) {
        Id host_id = fmt::format("{}_host-{}", edge->get_id(), host_num);
        std::shared_ptr<IHost> host = parse_host(params.hosts, host_id);
        ctx.hosts.emplace_back(host);

        add_links_between(host, edge, params.edge_host_links,
                          ctx.links.edge_host);
    }
}

static void add_edge_and_below(std::size_t k,
                               const Storage<ISwitch>& aggr_switches,
                               std::string_view pod_prefix,
                               FatTreeParams params,
                               FatTreeTopologyContext& ctx) {
    std::size_t edge_count = k / 2;
    std::size_t hosts_per_edge_count = k / 2;

    for (std::size_t edge_num = 1; edge_num <= edge_count; edge_num++) {
        Id edge_id = fmt::format("{}_edge-{}", pod_prefix, edge_num);
        std::shared_ptr<ISwitch> edge =
            parse_switch(params.aggr_switches, params.packet_spraying, edge_id);

        ctx.switches.edge.emplace_back(edge);

        // add aggr<->edge links
        for (auto& aggr : aggr_switches) {
            add_links_between(aggr, edge, params.aggr_edge_links,
                              ctx.links.aggr_edge);
        }

        // add hosts
        add_hosts_and_their_links(hosts_per_edge_count, edge, params, ctx);
    }
}

static void add_aggr_and_below(std::size_t k, std::size_t pod_num,
                               FatTreeParams params,
                               FatTreeTopologyContext& ctx) {
    std::size_t ports_half = k / 2;
    std::size_t aggr_per_pod_count = k / 2;
    std::string pod_prefix = fmt::format("pod-{}", pod_num);

    // add aggr switches & its uplinks
    Storage<ISwitch> aggr_switches;
    for (std::size_t aggr_num = 1; aggr_num <= aggr_per_pod_count; aggr_num++) {
        Id aggr_id = fmt::format("{}_aggr-{}", pod_prefix, aggr_num);
        std::shared_ptr<ISwitch> aggr =
            parse_switch(params.aggr_switches, params.packet_spraying, aggr_id);

        aggr_switches.emplace_back(aggr);

        // add core<->aggr links
        std::size_t core_start_index = (aggr_num - 1) * ports_half;
        for (std::size_t core_num = core_start_index;
             core_num < core_start_index + ports_half; core_num++) {
            add_links_between(aggr, ctx.switches.core[core_num],
                              params.core_aggr_links, ctx.links.core_aggr);
        }
    }

    for (auto& swtch : aggr_switches) {
        ctx.switches.aggr.emplace_back(swtch);
    }

    // add edge switches & its uplinks
    add_edge_and_below(k, aggr_switches, pod_prefix, params, ctx);
}

}  // namespace sim