#include "logger/logger.hpp"
#include <initializer_list>
#include <memory>
#include <utility>
#include <ctime>
#include <cstdlib> 

#include "device/device.hpp"
#include "express_pass/express_pass_flow.hpp"
#include "express_pass/express_pass_link.hpp"
#include "express_pass/express_pass_receiver.hpp"
#include "express_pass/express_pass_sender.hpp"
#include "simulator.hpp"


#include <fstream>
#include <iostream>

namespace sim {

using EPSimulator = Simulator<ExpressPassSender, Switch, ExpressPassReceiver, ExpressPassFlow, ExpressPassLink>;
    
using two_way_link_t = std::pair<std::shared_ptr<sim::IRoutingDevice>,
std::shared_ptr<sim::IRoutingDevice>>;
    
void add_two_way_links(sim::EPSimulator& sim,
                       std::initializer_list<two_way_link_t> links) {
    for (auto& l : links) {
        sim.add_link(l.first, l.second, 10, 0);
        sim.add_link(l.second, l.first, 10, 0);
    }
}

}

int main() {
    // std::ofstream outFile("results.txt");

    // std::streambuf* coutBackup = std::cout.rdbuf();
    // std::cout.rdbuf(outFile.rdbuf());

    std::srand(std::time(0));

    sim::EPSimulator sim;
    std::vector<std::shared_ptr<sim::ExpressPassSender>> senders;
    
    const int sender_count = 100;
    for (int i = 1; i <= sender_count; ++i) {
        senders.push_back(sim.add_sender("sender" + std::to_string(i)));
        
    }
    
    auto swtch = sim.add_switch("switch");
    auto receiver = sim.add_receiver("receiver");
    
    // Create links
    std::vector<std::pair<std::shared_ptr<sim::Sender>, std::shared_ptr<sim::Switch>>> links;
    for (auto sender : senders) {
        add_two_way_links(sim, {{sender, swtch}});
    }
    add_two_way_links(sim, {{swtch, receiver}});
    
    constexpr Time delay_between_packets = 10;
    constexpr Time stop_time = 100000;
    constexpr Size packet_size = 1538;
    constexpr std::uint32_t packets_per_flow = 100;
    
    for (size_t i = 0; i < senders.size(); i++) {
        sim.add_flow(senders[i], receiver, packet_size,
                                    delay_between_packets, packets_per_flow + 1);
    }
    
    sim.start(stop_time);

    // std::cout.rdbuf(coutBackup);
    
    // outFile.close();

    return 0;
}
