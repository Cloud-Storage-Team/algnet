#include "NetworkSimulator.hpp"

#include <memory>
#include <iostream>

int main() {
    NetworkSimulator ns;

    // Switch
    std::shared_ptr<Switch> s = std::make_shared<Switch>();

    // Senders
    std::shared_ptr<Server> fst_sender = std::make_shared<Server>(DeviceType::ServerSender);
    std::shared_ptr<Server> snd_sender = std::make_shared<Server>(DeviceType::ServerSender);

    // Receiver
    std::shared_ptr<Server> receiver = std::make_shared<Server>(DeviceType::ServerReceiver);

    // Add devices to simulator
    ns.devices.push_back(s);
    ns.devices.push_back(fst_sender);
    ns.devices.push_back(snd_sender);
    ns.devices.push_back(receiver);

    // Add flows to simulator
    NetworkSimulator::flows.emplace_back(new Flow({fst_sender, s, receiver}, {500, 1000}));
    NetworkSimulator::flows.emplace_back(new Flow({snd_sender, s, receiver}, {300, 1000}));

    ns.Run();
}
