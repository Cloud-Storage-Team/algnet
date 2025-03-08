#include "utils.hpp"

namespace test {

std::vector<std::shared_ptr<sim::RoutingModule>> createRoutingModules(size_t count) {
    std::vector<std::shared_ptr<sim::RoutingModule>> modules;
    for (size_t i = 0; i < count; ++i) {
        modules.emplace_back(std::make_shared<sim::RoutingModule>());
    }
    return modules;
}

TestLink::TestLink(std::shared_ptr<sim::IRoutingDevice> a_src,
                   std::shared_ptr<sim::IRoutingDevice> a_dest)
    : sim::Link(a_src, a_dest, 0, 0) {}

}  // namespace test
