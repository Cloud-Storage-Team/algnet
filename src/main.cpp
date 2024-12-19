#include "NetworkSimulator.hpp"

#include <memory>

int main() {
    NetworkSimulator ns;

    // Sender
    std::shared_ptr<Sender> sender = std::make_shared<Sender>(1500);

    // Switch
    std::shared_ptr<Switch> s = std::make_shared<Switch>(1000);

    // Receiver
    std::shared_ptr<Receiver> receiver = std::make_shared<Receiver>(1000);

    // Add devices to simulator
    NetworkSimulator::AddDevice(s);
    NetworkSimulator::AddDevice(sender);
    NetworkSimulator::AddDevice(receiver);

    // Add flows to simulator
    NetworkSimulator::EmplaceFlow({sender, s, receiver}, {500, 1000});

    // Turn on ACKs
    NetworkSimulator::EnableACK = true;
    ns.StopAt(Time::Seconds(1));

    ns.Run();
}
