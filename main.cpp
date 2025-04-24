#include "logger/logger.hpp"
#include "utils/graph_configuration_parcer.hpp"

int main() {
    sim::parse_configuration(
        "/home/ralnikovp/projects/algnet/configuration_example/examples/"
        "bus_topology.yml");

    return 0;
}
