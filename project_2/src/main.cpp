#include "simulator.hpp"
#include "express_pass_server.hpp"
#include "express_pass_switch.hpp"

#include <iostream>

int main(int argc, char* argv[]) {
    srand(static_cast<unsigned>(time(NULL)));

    std::uint32_t amount_of_senders = 10;
    std::uint64_t simulation_duration = 10000000;

    std::vector<std::shared_ptr<ServerBase>> senders;
    std::uint64_t receiver_id = NetworkSimulator::GenerateNewID();
    std::vector<std::uint64_t> senders_ids;

    for (int sender_index = 0; sender_index < amount_of_senders; sender_index++) {
        std::uint64_t id = NetworkSimulator::GenerateNewID();
        senders.emplace_back(new ExpressPassSender(receiver_id, id, simulation_duration, 100));
        senders_ids.push_back(id);
    }
    
    std::shared_ptr<ServerBase> receiver(new ExpressPassReceiver(std::move(senders_ids), receiver_id, simulation_duration, ExpressPass::default_inter_credit_gap_ns));
    std::shared_ptr<NetworkSwitch> n_switch(new ExpressPassSwitch(ExpressPass::default_credit_rate_limit_ns));
    NetworkSimulator simulator(senders, receiver, n_switch, simulation_duration, 50);

    simulator.StartSimulation();
    std::cout << receiver->ToString();
}
