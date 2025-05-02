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
    std::srand(std::time(0));

    sim::EPSimulator sim;
    auto sender1 = sim.add_sender("sender1");
    auto sender2 = sim.add_sender("sender2");
    auto sender3 = sim.add_sender("sender3");
    auto sender4 = sim.add_sender("sender4");
    auto sender5 = sim.add_sender("sender5");
    auto sender6 = sim.add_sender("sender6");
    auto sender7 = sim.add_sender("sender7");
    auto sender8 = sim.add_sender("sender8");
    auto sender9 = sim.add_sender("sender9");
    auto sender10 = sim.add_sender("sender10");
    auto swtch = sim.add_switch("switch");
    auto receiver = sim.add_receiver("receiver");

    add_two_way_links(sim, {{sender1, swtch},
                            {sender2, swtch},
                            {sender3, swtch},
                            {sender4, swtch},
                            {sender5, swtch},
                            {sender6, swtch},
                            {sender7, swtch},
                            {sender8, swtch},
                            {sender9, swtch},
                            {sender10, swtch},
                            {swtch, receiver}});

    constexpr Time delay_between_packets = 100;
    constexpr Time stop_time = 1000000;
    constexpr Size packet_size = 1538;

    constexpr std::uint32_t packets_to_send_by_flow1 = 1000;
    auto flow1 = sim.add_flow(sender1, receiver, packet_size,
                              delay_between_packets, packets_to_send_by_flow1);

    constexpr std::uint32_t packets_to_send_by_flow2 = 1000;
    auto flow2 = sim.add_flow(sender2, receiver, packet_size,
                              delay_between_packets, packets_to_send_by_flow2);

    constexpr std::uint32_t packets_to_send_by_flow3 = 1000;
    auto flow3 = sim.add_flow(sender3, receiver, packet_size,
                              delay_between_packets, packets_to_send_by_flow3);


    constexpr std::uint32_t packets_to_send_by_flow4 = 1000;
    auto flow4 = sim.add_flow(sender4, receiver, packet_size,
                              delay_between_packets, packets_to_send_by_flow4);

    constexpr std::uint32_t packets_to_send_by_flow5 = 1000;
    auto flow5 = sim.add_flow(sender5, receiver, packet_size,
                              delay_between_packets, packets_to_send_by_flow5);

    constexpr std::uint32_t packets_to_send_by_flow6 = 1000;
    auto flow6 = sim.add_flow(sender6, receiver, packet_size,
                              delay_between_packets, packets_to_send_by_flow6);


    constexpr std::uint32_t packets_to_send_by_flow7 = 1000;
    auto flow7 = sim.add_flow(sender7, receiver, packet_size,
                              delay_between_packets, packets_to_send_by_flow7);

    constexpr std::uint32_t packets_to_send_by_flow8 = 1000;
    auto flow8 = sim.add_flow(sender8, receiver, packet_size,
                              delay_between_packets, packets_to_send_by_flow8);

    constexpr std::uint32_t packets_to_send_by_flow9 = 1000;
    auto flow9 = sim.add_flow(sender9, receiver, packet_size,
                              delay_between_packets, packets_to_send_by_flow9);

    constexpr std::uint32_t packets_to_send_by_flow10 = 1000;
    auto flow10 = sim.add_flow(sender10, receiver, packet_size,
                              delay_between_packets, packets_to_send_by_flow10);    

    sim.start(stop_time);
    return 0;
}
