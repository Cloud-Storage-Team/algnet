#include "bfc_switch.hpp"
#include "server.hpp"
#include "simulator.hpp"

int main(int argc, char *argv[]) {
    std::uint32_t amount_of_senders = 10;

    std::vector<std::shared_ptr<ServerBase>> senders;
    std::vector<std::uint64_t> senders_ids;

    for (int sender_index = 0; sender_index < amount_of_senders;
         sender_index++) {
        std::uint64_t id = NetworkSimulator::GenerateNewID();
        senders.emplace_back(new ServerSender(10, id));
        senders_ids.push_back(id);
    }

    std::shared_ptr<ServerBase> receiver(
        new ServerReceiver(senders_ids, NetworkSimulator::GenerateNewID()));
    std::shared_ptr<NetworkSwitch> n_switch_1(new BFCSwitch());
    std::shared_ptr<NetworkSwitch> n_switch_2(new BFCSwitch());

    NetworkSimulator simulator(senders, receiver, n_switch_1, n_switch_2,
                               1000000);

    simulator.StartSimulation();
}