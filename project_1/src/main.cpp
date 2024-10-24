#include "simulator.hpp"

#include <iostream>
#include <queue>

int main(int argc, char* argv[]) {
    std::uint32_t number_of_servers = 3;
    NetworkSimulator simulator(number_of_servers);

    std::vector<std::uint32_t> distances_to_receiver = {10, 4, 20};
    simulator.StartSimulation(distances_to_receiver);

    simulator.PrintConfiguration();
}
