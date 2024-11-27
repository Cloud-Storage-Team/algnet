#include "NetworkSimulator.hpp"

int main() {
    std::vector<std::uint32_t> distances_to_receiver_ns = {500, 1000, 2000};

    NetworkSimulator ns(distances_to_receiver_ns);
    ns.Run();
}
