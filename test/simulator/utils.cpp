#include "utils.hpp"

#include "simulator.hpp"

namespace test {

std::shared_ptr<sim::Sender> TestSimulator::get_sender(Id id) {
    return _get_sender(id);
}

std::shared_ptr<sim::Switch> TestSimulator::get_switch(Id id) {
    return _get_switch(id);
}

std::shared_ptr<sim::Receiver> TestSimulator::get_receiver(Id id) {
    return _get_receiver(id);
}

std::shared_ptr<sim::Flow> TestSimulator::get_flow(Id id) {
    return _get_flow(id);
}

void add_two_way_links(sim::BasicSimulator& sim,
                       std::initializer_list<two_way_link_t> links) {
    static int counter = 1;
    const std::string link_name = "test_link";
    for (auto& l : links) {
        sim.add_link(fmt::format("{}_{}", link_name, counter++), l.first,
                     l.second, 0, 0);
        sim.add_link(fmt::format("{}_{}", link_name, counter++), l.second,
                     l.first, 0, 0);
    }
}

static std::shared_ptr<sim::IRoutingDevice> get_next_device(
    std::shared_ptr<sim::IRoutingDevice> curr_device,
    std::shared_ptr<sim::IRoutingDevice> dest_device) {
    auto next_link = curr_device->get_link_to_destination(dest_device);
    if (next_link == nullptr) {
        return nullptr;
    }
    return next_link->get_to();
}

bool check_reachability(std::shared_ptr<sim::IRoutingDevice> src_device,
                        std::shared_ptr<sim::IRoutingDevice> dest_device) {
    std::set<std::shared_ptr<sim::IRoutingDevice>> used;
    auto curr_device = src_device;
    while (curr_device != dest_device) {
        if (curr_device == nullptr || used.contains(curr_device)) {
            return false;
        }
        used.insert(curr_device);
        curr_device = get_next_device(curr_device, dest_device);
    }
    return true;
}

}  // namespace test
