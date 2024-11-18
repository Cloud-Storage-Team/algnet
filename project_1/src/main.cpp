#include "NetworkSimulator.hpp"

#include <iostream>

int main(int argc, char* argv[]) {
    std::uint32_t simulation_time_sec = 30;
    std::vector<std::uint32_t> distances_to_receiver_us = {10, 4, 20};

    NetworkSimulator simulator(distances_to_receiver_us);

    simulator.StartSimulation(simulation_time_sec);

    std::cout << simulator;
}