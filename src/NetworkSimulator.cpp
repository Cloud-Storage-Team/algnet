#include "NetworkSimulator.hpp"
#include "Flow.hpp"

#include <set>
#include <utility>
#include <functional>
#include <stack>

void NetworkSimulator::Run() {
    /* Create map: id --> device */
    for (std::uint32_t i = 0; i < devices.size(); ++i) {
        device_by_id[devices[i]->id] = i;
    }
    /* Topological sorting of the network graph */
    Topsort();

    for (std::uint32_t i = 0; i < iteration_num; ++i) {

    }
}

/**
 * Network graph.
 * Network devices are vertices, links between them are edges.
 * Each link in network is a pair of forward and backward graph edges.
 * Forward traversal -- from senders to receivers, backward traversal -- otherwise.
 */
void NetworkSimulator::Topsort() {
    std::set<std::pair<std::uint32_t, std::uint32_t>> forward_edges {};
    std::set<std::pair<std::uint32_t, std::uint32_t>> backward_edges {};

    /* Add all forward edges to set */
    for (std::unique_ptr<Flow>& flow: flows) {
        for (std::uint32_t i = 0; i < flow->path.size() - 1; ++i) {
            forward_edges.emplace(flow->path[i]->id, flow->path[i + 1]->id);
        }
    }

    /* Reverse each edge to get backward edges */
    for (auto [u, v]: forward_edges) {
        backward_edges.emplace(v, u);
    }

    /* Topological sorting function, result_traversal is a topsorted network graph traversal */
    auto topsort = [&](std::set<std::pair<std::uint32_t, std::uint32_t>>& edges,
                                                       std::vector<std::uint32_t>& result_traversal) {
        /* Create graph adjacency list */
        std::vector<std::vector<std::uint32_t>> adj(devices.size());
        for (auto [u, v] : edges) {
            adj[u].push_back(v);
        }

        std::vector<bool> visited(devices.size(), false);
        std::stack<std::uint32_t> stack;

        /* DFS for graph traversal */
        std::function<void(std::uint32_t)> dfs = [&](std::uint32_t vertex) {
            visited[vertex] = true;
            for (std::uint32_t adj_vertex: adj[vertex]) {
                if (!visited[adj_vertex]) {
                    dfs(adj_vertex);
                }
            }
            stack.push(vertex);
        };

        /* Topological sorting */
        for (int v = 0; v < devices.size(); ++v) {
            if (!visited[v]) {
                dfs(v);
            }
        }

        /*  */
        while (!stack.empty()) {
            result_traversal.push_back(stack.top());
            stack.pop();
        }
    };

    /* Topsort forward and backward traversals */
    topsort(forward_edges, forward_device_visit_order);
    topsort(backward_edges, backward_device_visit_order);
}
