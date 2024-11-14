#include "simulator.hpp"
#include "express_pass_server.hpp"
#include "express_pass_switch.hpp"

int main(int argc, char* argv[]) {
    std::uint32_t amount_of_senders = 10;

    std::vector<std::shared_ptr<ServerBase>> senders;
    std::vector<std::uint64_t> senders_ids;

    for (int sender_index = 0; sender_index < amount_of_senders; sender_index++) {
        std::uint64_t id = NetworkSimulator::GenerateNewID();
        senders.emplace_back(new ExpressPassSender(id, 100));
        senders_ids.push_back(id);
    }
    
    std::shared_ptr<ServerBase> receiver(new ExpressPassReciever(senders_ids, NetworkSimulator::GenerateNewID(), ExpressPass::default_inter_credit_gap_ns));
    std::shared_ptr<NetworkSwitch> n_switch(new ExpressPassSwitch(ExpressPass::default_credit_rate_limit_ns));
    NetworkSimulator simulator(senders, receiver, n_switch, 1000000000);

    simulator.StartSimulation();
}
