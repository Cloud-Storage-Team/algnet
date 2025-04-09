#include "utils.hpp"

#include "logger.hpp"
#include "simulator.hpp"

namespace test {

void add_two_way_links(sim::Simulator& sim,
               std::initializer_list<two_way_link_t> links) {
    for (auto& l : links) {
        sim.add_link(l.first, l.second, 0, 0);
        sim.add_link(l.second, l.first, 0, 0);
    }
}

bool check_reachability(std::shared_ptr<sim::IRoutingDevice> src_device,
                       std::shared_ptr<sim::IRoutingDevice> dest_device) {
    auto curr_device =
        src_device->get_link_to_destination(dest_device)->get_to();
    while (curr_device != dest_device) {
        if (curr_device == nullptr) {
            return false;
        }
        curr_device =
            curr_device->get_link_to_destination(dest_device)->get_to();
    }
    return true;
}

}  // namespace test
