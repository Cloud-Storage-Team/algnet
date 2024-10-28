#include "simulator.hpp"

#include <iostream>
#include <queue>

int main(int argc, char* argv[]) {
    NetworkSimulator simulator({10, 4, 20});

    simulator.StartSimulation();

    std::cout << simulator;
}
