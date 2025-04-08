#pragma once

#include <initializer_list>
#include <memory>
#include <utility>

#include "device.hpp"
#include "simulator.hpp"

namespace test {

using two_way_link_t = std::pair<std::shared_ptr<sim::IRoutingDevice>,
                                 std::shared_ptr<sim::IRoutingDevice>>;

void add_links(sim::Simulator& sim,
               std::initializer_list<two_way_link_t> links);

}  // namespace test
