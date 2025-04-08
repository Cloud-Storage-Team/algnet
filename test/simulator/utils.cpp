#include "utils.hpp"

#include "simulator.hpp"

namespace test {

void add_links(sim::Simulator& sim,
               std::initializer_list<two_way_link_t> links) {
    for (auto& l : links) {
        sim.add_link(l.first, l.second, 0, 0);
        sim.add_link(l.second, l.first, 0, 0);
    }
}

}  // namespace test
