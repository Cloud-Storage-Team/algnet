#pragma once

#include <initializer_list>
#include <memory>
#include <utility>

#include "simulator.hpp"

namespace test {

using two_way_link_t = std::pair<std::shared_ptr<sim::IDevice>,
                                 std::shared_ptr<sim::IDevice>>;

void add_two_way_links(sim::BasicSimulator& sim,
                       std::initializer_list<two_way_link_t> links);

bool check_reachability(std::shared_ptr<sim::IDevice> src_device,
                        sim::Packet packet_to_dest);

}  // namespace test
