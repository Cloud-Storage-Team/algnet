#include "NetworkSimulator.hpp"

#include <memory>

int main() {
    NetworkSimulator ns;

    // Sender
    std::shared_ptr<Sender> sender = std::make_shared<Sender>(100);

    // Switch
    std::shared_ptr<Switch> s = std::make_shared<Switch>(100);

    // Receiver
    std::shared_ptr<Receiver> receiver = std::make_shared<Receiver>(100);

    // Add devices to simulator
    NetworkSimulator::AddDevice(s);
    NetworkSimulator::AddDevice(sender);
    NetworkSimulator::AddDevice(receiver);

    // Add flows to simulator
    NetworkSimulator::EmplaceFlow({sender, s, receiver}, {10, 10});

    // Turn on ACKs
    NetworkSimulator::EnableACK = true;
    ns.StopAt(Time::Seconds(1));

    ns.Run();
}
