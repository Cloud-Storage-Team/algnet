#pragma once

#include <initializer_list>
#include <memory>
#include <utility>

#include "device/device.hpp"
#include "device/sender.hpp"
#include "simulator.hpp"
#include "utils/identifier_factory.hpp"

namespace test {

class TestSimulator : public sim::BasicSimulator {
public:
    std::shared_ptr<sim::Sender> get_sender(Id id);
    std::shared_ptr<sim::Switch> get_switch(Id id);
    std::shared_ptr<sim::Receiver> get_receiver(Id id);
    std::shared_ptr<sim::Flow> get_flow(Id id);
};

using two_way_link_t = std::pair<std::shared_ptr<sim::Identifiable>,
                                 std::shared_ptr<sim::Identifiable>>;

void add_two_way_links(sim::BasicSimulator& sim,
                       std::initializer_list<two_way_link_t> links);

bool check_reachability(std::shared_ptr<sim::IRoutingDevice> src_device,
                        std::shared_ptr<sim::IRoutingDevice> dest_device);

}  // namespace test
