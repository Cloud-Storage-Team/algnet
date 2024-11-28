#include <unordered_map>

#include "link.hpp"
#include "routing_packet.hpp"
#include "utils.hpp"

RoutingNetworkElement::RoutingNetworkElement(
    std::unordered_map<std::uint64_t, std::shared_ptr<NetworkElement>>
        &routing_table)
    : routing_table(routing_table) {
}

void RoutingNetworkElement::AddNewRout(std::uint64_t destination_id,
                                       std::shared_ptr<NetworkElement> &link) {
    routing_table[destination_id] = link;
}

std::shared_ptr<NetworkElement>
RoutingNetworkElement::GetNextElement(std::uint64_t destination_id) {
    return routing_table[destination_id];
}
