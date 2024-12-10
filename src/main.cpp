#include "NetworkSimulator.hpp"

#include <memory>
#include <iostream>

int main() {
    NetworkSimulator ns;

    // Switch
    std::shared_ptr<Switch> s = std::make_shared<Switch>();

    // Senders
    std::shared_ptr<Server> fst_sender = std::make_shared<Server>();
    std::shared_ptr<Server> snd_sender = std::make_shared<Server>();

    // Receiver
    std::shared_ptr<Server> receiver = std::make_shared<Server>();

    // Add neighbors to devices
    s->adjacent_devices.push_back(fst_sender);
    s->adjacent_devices.push_back(receiver);
    fst_sender->adjacent_devices.push_back(s);
    snd_sender->adjacent_devices.push_back(s);
    receiver->adjacent_devices.push_back(s);

    // Add devices to simulator
    ns.devices.push_back(s);
    ns.devices.push_back(fst_sender);
    ns.devices.push_back(snd_sender);
    ns.devices.push_back(receiver);

    // Add flows to simulator
    ns.flows.emplace_back(new Flow({fst_sender, s, receiver}));
    ns.flows.back()->distances_ns = {500, 1000};

    ns.flows.emplace_back(new Flow({snd_sender, s, receiver}));
    ns.flows.back()->distances_ns = {300, 1000};

    std::cout << "Switch ID: " << s->id << std::endl;
    std::cout << "Sender ID: " << fst_sender->id << std::endl;
    std::cout << "Sender2 ID: " << snd_sender->id << std::endl;
    std::cout << "Receiver ID: " << receiver->id << std::endl;

    ns.Run();
}
