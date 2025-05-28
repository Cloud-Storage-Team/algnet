#include "utils.hpp"

#include "simulator.hpp"

namespace test {

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
