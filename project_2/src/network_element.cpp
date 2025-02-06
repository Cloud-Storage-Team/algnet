#include "network_element.hpp"

#include <unordered_map>

NetworkElement::NetworkElement(const std::unordered_map<std::uint64_t, std::shared_ptr<Link>>& routing_table):
    routing_table(routing_table) {}

void NetworkElement::AddNewRoute(std::uint64_t destination_id, std::shared_ptr<Link>& link) {
    routing_table[destination_id] = link;
}

std::shared_ptr<Link> NetworkElement::GetNextElement(std::uint64_t destination_id) {
    if (routing_table.find(destination_id) == routing_table.end()) {
        throw std::runtime_error("Routing table error");
    }
    return routing_table[destination_id];
}
