#include "BFCSwitch.hpp"
#include "NetworkSimulator.hpp"
#include <memory>

int main() {
    NetworkSimulator ns;

    // Sender
    std::shared_ptr<Sender> sender = std::make_shared<Sender>(100); 

    // Receiver
    std::shared_ptr<Receiver> receiver = std::make_shared<Receiver>(100); 
    
    // Switch
    std::vector<std::shared_ptr<Switch>> switches;
    std::shared_ptr<Switch> s = std::make_shared<Switch>(100); 
    switches.push_back(s);
    NetworkSimulator::AddNewFlow(sender, receiver, switches);



    // Turn on ACKs
    NetworkSimulator::EnableACK = true;
    ns.StopAt(Time::Milliseconds(300));
    NetworkSimulator::logger.Setup("../rtt.txt", "../queue_size.txt");

    ns.Run();
}
