#include "NetworkSimulator.hpp"

#include <iostream>

int main(int argc, char* argv[]) {
    std::uint32_t simulation_iterations = 25;
    std::vector<std::uint32_t> data_rates_gbps = {1, 5, 3};

    NetworkSimulator simulator(data_rates_gbps);

    simulator.StartSimulation(simulation_iterations);

    std::cout << simulator;
}