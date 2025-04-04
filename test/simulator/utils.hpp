#pragma once

#include <initializer_list>
#include <memory>
#include <utility>

#include "device.hpp"
#include "simulator.hpp"

namespace test {

void add_links(
    sim::Simulator& sim,
    std::initializer_list<std::pair<std::shared_ptr<sim::IRoutingDevice>,
                                    std::shared_ptr<sim::IRoutingDevice>>>
        links);

}  // namespace test
