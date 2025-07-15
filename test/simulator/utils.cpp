#include "utils.hpp"

#include "simulator.hpp"

namespace test {

void add_two_way_links(sim::BasicSimulator& sim,
                       std::initializer_list<two_way_link_t> links) {
    for (auto& l : links) {
        sim.add_link(std::make_shared<sim::Link>("", l.first, l.second, 0, 0));
        sim.add_link(std::make_shared<sim::Link>("", l.second, l.first, 0, 0));
    }
}

static std::shared_ptr<sim::IDevice> get_next_device(
    std::shared_ptr<sim::IDevice> curr_device,
    sim::Packet packet_to_dest) {
    auto next_link = curr_device->get_link_to_destination(packet_to_dest);
    if (next_link == nullptr) {
        return nullptr;
    }
    return next_link->get_to();
}

bool check_reachability(std::shared_ptr<sim::IDevice> src_device,
                        sim::Packet packet_to_dest) {
    std::set<std::shared_ptr<sim::IDevice>> used;
    auto curr_device = src_device;
    while (curr_device->get_id() != packet_to_dest.dest_id) {
        if (curr_device == nullptr || used.contains(curr_device)) {
            return false;
        }
        used.insert(curr_device);
        curr_device = get_next_device(curr_device, packet_to_dest);
    }
    return true;
}

}  // namespace test
