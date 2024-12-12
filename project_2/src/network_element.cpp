#include "routing_packet.hpp"
#include "link.hpp"

#include <unordered_map>

RoutingNetworkElement::RoutingNetworkElement(const std::unordered_map<std::uint64_t, std::shared_ptr<NetworkElement>>& routing_table):
    routing_table(routing_table) {}

void RoutingNetworkElement::AddNewRoute(std::uint64_t destination_id, std::shared_ptr<NetworkElement>& link) {
    routing_table[destination_id] = link;
}

std::shared_ptr<NetworkElement> RoutingNetworkElement::GetNextElement(std::uint64_t destination_id) {
    if (routing_table.find(destination_id) == routing_table.end()) {
        throw std::runtime_error("Routing table error");
    }
    return routing_table[destination_id];
}
