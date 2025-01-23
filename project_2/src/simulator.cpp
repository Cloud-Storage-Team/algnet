#include "simulator.hpp"
#include <iostream>

std::uint64_t NetworkSimulator::id_to_give = 0;

NetworkSimulator::NetworkSimulator(std::vector<std::shared_ptr<ServerBase>>& senders, std::shared_ptr<ServerBase> receiver, std::shared_ptr<NetworkSwitch> n_switch, std::uint64_t simulation_duration_ns, std::uint64_t links_speed):
    receiver(receiver),
    n_switch(n_switch),
    simulation_duration_ns(simulation_duration_ns)
{
    std::shared_ptr<NetworkElement> routing_receiver = std::dynamic_pointer_cast<NetworkElement>(receiver);
    std::shared_ptr<NetworkElement> routing_switch = std::dynamic_pointer_cast<NetworkElement>(n_switch);
    auto recv_switch_conn = AddNewConnection(routing_receiver, routing_switch, links_speed);
    
    routing_switch->AddNewRoute(receiver->GetID(), recv_switch_conn.reversed_link);

    for (std::shared_ptr<ServerBase> sender : senders) {
        ids_to_senders[sender->GetID()] = sender;
        std::shared_ptr<NetworkElement> routing_sender = std::dynamic_pointer_cast<NetworkElement>(sender);
        auto switch_sender_conn = AddNewConnection(routing_switch, routing_sender, links_speed);
        routing_switch->AddNewRoute(sender->GetID(), switch_sender_conn.direct_link);
        routing_sender->AddNewRoute(receiver->GetID(), switch_sender_conn.reversed_link);
        routing_receiver->AddNewRoute(sender->GetID(), recv_switch_conn.direct_link);
    }
}

Connection NetworkSimulator::AddNewConnection(std::shared_ptr<NetworkElement> source, std::shared_ptr<NetworkElement> destination, std::uint64_t speed) {
    std::shared_ptr<NetworkElement> base_source = std::dynamic_pointer_cast<NetworkElement>(source);
    std::shared_ptr<NetworkElement> base_destination = std::dynamic_pointer_cast<NetworkElement>(destination);
    std::shared_ptr<Link> link = std::make_shared<Link>(Link(base_source, base_destination, speed));
    std::shared_ptr<Link> reverse_link = std::make_shared<Link>(Link(base_destination, base_source, speed));
    
    links.emplace_back(link);
    links.emplace_back(reverse_link);

    return Connection(std::move(link), std::move(reverse_link));
}

void NetworkSimulator::GenerateNewEvents() {
    next_ask = simulation_duration_ns + 1;

    std::uint64_t possible_next_ask = receiver->SendPackets(current_time_ns, events);
    next_ask = std::min(next_ask, ((possible_next_ask == 0) ? next_ask : possible_next_ask));

    for (auto& [_, sender] : ids_to_senders) {
        possible_next_ask = sender->SendPackets(current_time_ns, events);   
        next_ask = std::min(next_ask, ((possible_next_ask == 0) ? next_ask : possible_next_ask));
    }
}

void NetworkSimulator::StartSimulation() {
    GenerateNewEvents();

    while (current_time_ns <= simulation_duration_ns && !events.empty()) {
        std::shared_ptr<Event> event(events.top());
        events.pop();

        current_time_ns = event->event_time;
        event->perform_action(events);
    }

    while (!events.empty()) {
        std::shared_ptr<Event> event(events.top());
        events.pop();

        current_time_ns = event->event_time;

        event->perform_action(events);
    }
}
