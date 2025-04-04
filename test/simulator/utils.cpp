#include "utils.hpp"

#include "simulator.hpp"

namespace test {

void add_links(
    sim::Simulator& s,
    std::initializer_list<std::pair<std::shared_ptr<sim::IRoutingDevice>,
                                    std::shared_ptr<sim::IRoutingDevice>>>
        links) {
    for (auto& l : links) {
        s.add_link(l.first, l.second, 0, 0);
        s.add_link(l.second, l.first, 0, 0);
    }
}

}  // namespace test
