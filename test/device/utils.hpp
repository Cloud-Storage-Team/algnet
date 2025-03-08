#pragma once
#include <gtest/gtest.h>

#include "../../source/device.hpp"
#include "../../source/link.hpp"

namespace test {

class Link;

std::vector<std::shared_ptr<sim::RoutingModule>> createRoutingModules(size_t count);

class TestLink : public sim::Link {
public:
    TestLink(std::shared_ptr<sim::IRoutingDevice> a_src,
             std::shared_ptr<sim::IRoutingDevice> a_dest);
    ~TestLink() = default;
};

}  // namespace test
