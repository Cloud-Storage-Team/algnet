#include "NetworkSimulator.hpp"

#include <memory>
#include <iostream>

int main() {
    NetworkSimulator ns;

    // Switch
    std::shared_ptr<Switch> s = std::make_shared<Switch>();

    // Sender
    std::shared_ptr<Server> sender = std::make_shared<Server>();

    // Receiver
    std::shared_ptr<Server> receiver = std::make_shared<Server>();

    // Add neighbors to devices
    s->adjacent_devices.push_back(sender);
    s->adjacent_devices.push_back(receiver);
    sender->adjacent_devices.push_back(s);
    receiver->adjacent_devices.push_back(s);

    // Add devices to simulator
    ns.devices.push_back(s);
    ns.devices.push_back(sender);
    ns.devices.push_back(receiver);

    // Add flows to simulator
    ns.flows.emplace_back(new Flow({sender, s, receiver}));
    ns.flows.front()->distances_ns = {500, 1000};

    std::cout << "Number of devices: " << ns.devices.size() << std::endl;
    std::cout << "Number of flows: " << ns.flows.size() << std::endl;

    ns.Run();
}
